#ifndef HASH_OPEN_MAP_HPP_
#define HASH_OPEN_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"


namespace ics
{


#ifndef undefinedhashdefined
#define undefinedhashdefined

    template< class T >
    int undefinedhash( const T& a )
    {
        return 0;
    }

#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
    template< class KEY, class T, int (* thash)( const KEY& a ) = undefinedhash<KEY>>
    class HashOpenMap
    {
    public:
        typedef ics::pair<KEY, T> Entry;

        typedef int (* hashfunc)( const KEY& a );

        //Destructor/Constructors
        ~HashOpenMap();

        HashOpenMap( double the_load_threshold = 1.0, int (* chash)( const KEY& a ) = undefinedhash<KEY> );

        explicit HashOpenMap( int initial_bins, double the_load_threshold = 1.0, int (* chash)( const KEY& k ) = undefinedhash<KEY> );

        HashOpenMap( const HashOpenMap<KEY, T, thash>& to_copy, double the_load_threshold = 1.0, int (* chash)( const KEY& a ) = undefinedhash<KEY> );

        explicit HashOpenMap( const std::initializer_list<Entry>& il, double the_load_threshold = 1.0, int (* chash)( const KEY& a ) = undefinedhash<KEY> );

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        explicit
        HashOpenMap( const Iterable& i, double the_load_threshold = 1.0, int (* chash)( const KEY& a ) = undefinedhash<KEY> );


        //Queries
        bool empty() const;

        int size() const;

        bool has_key( const KEY& key ) const;

        bool has_value( const T& value ) const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<


        //Commands
        T put( const KEY& key, const T& value );

        T erase( const KEY& key );

        void clear();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        int put_all( const Iterable& i );


        //Operators

        T& operator[]( const KEY& );

        const T& operator[]( const KEY& ) const;

        HashOpenMap<KEY, T, thash>& operator=( const HashOpenMap<KEY, T, thash>& rhs );

        bool operator==( const HashOpenMap<KEY, T, thash>& rhs ) const;

        bool operator!=( const HashOpenMap<KEY, T, thash>& rhs ) const;

        template< class KEY2, class T2, int (* hash2)( const KEY2& a ) >
        friend std::ostream& operator<<( std::ostream& outs, const HashOpenMap<KEY2, T2, hash2>& m );


    public:
        class Iterator
        {
        public:
            //Private constructor called in begin/end, which are friends of HashOpenMap<T>
            ~Iterator();

            Entry erase();

            std::string str() const;

            HashOpenMap<KEY, T, thash>::Iterator& operator++();

            HashOpenMap<KEY, T, thash>::Iterator operator++( int );

            bool operator==( const HashOpenMap<KEY, T, thash>::Iterator& rhs ) const;

            bool operator!=( const HashOpenMap<KEY, T, thash>::Iterator& rhs ) const;

            Entry& operator*() const;

            Entry* operator->() const;

            friend std::ostream& operator<<( std::ostream& outs, const HashOpenMap<KEY, T, thash>::Iterator& i )
            {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }

            friend Iterator HashOpenMap<KEY, T, thash>::begin() const;

            friend Iterator HashOpenMap<KEY, T, thash>::end() const;

        private:
            //If can_erase is false, must ++ to reach next value
            int current; //Bin Index
            HashOpenMap<KEY, T, thash>* ref_map;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator( HashOpenMap<KEY, T, thash>* iterate_over, int initial );
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        int (* hash)( const KEY& k );      //Hashing function used (from template or constructor)
        enum bin_state
        {
            bs_empty, bs_occupied, bs_was_occupied
        };
        Entry* map = nullptr;    //Entry array
        bin_state* state = nullptr;    //bin_state[] describes the state of map[i]
        double load_threshold;         //used/bins <= load_threshold
        int bins = 1;          //# bins in array (should start at 1 so hash_compress doesn't % 0)
        int used = 0;          //Cache for number of key->value pairs in the hash table
        int mod_count = 0;          //For sensing concurrent modification


        //Helper methods
        int hash_compress( const KEY& key ) const;  //hash function ranged to [0,bins-1]
        int find_key( const KEY& key ) const;  //Returns index to key's bin or -1
        int next_unoccupied( int bin ) const;  //this bin or next beyond that is unoccupied

        void ensure_load_threshold( int new_used );                 //Reallocate if load_factor > load_threshold
    };




////////////////////////////////////////////////////////////////////////////////
//
//HashOpenMap class and related definitions

//Destructor/Constructors

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::~HashOpenMap()
    {
        this->clear();
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::HashOpenMap( double the_load_threshold, int (* chash)( const KEY& k ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<KEY> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<KEY> )
            throw TemplateFunctionError( "HashOpenMap::default constructor: neither specified" );
        if ( thash != ( hashfunc ) undefinedhash<KEY> && chash != ( hashfunc ) undefinedhash<KEY> && thash != chash )
            throw TemplateFunctionError( "HashOpenMap::default constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenMap::default constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        map = new Entry[1];
        state = new bin_state[1]();
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::HashOpenMap( int initial_bins, double the_load_threshold, int (* chash)( const KEY& k ))
            : bins( initial_bins ), load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<KEY> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<KEY> )
            throw TemplateFunctionError( "HashOpenMap::initial_bins constructor: neither specified" );
        if ( thash != ( hashfunc ) undefinedhash<KEY> && chash != ( hashfunc ) undefinedhash<KEY> && thash != chash )
            throw TemplateFunctionError( "HashOpenMap::initial_bins constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenMap::initial_bins constructor: load_threshold is > 1" );

        map = new Entry[initial_bins];
        state = new bin_state[initial_bins]();
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::HashOpenMap( const HashOpenMap<KEY, T, thash>& to_copy, double the_load_threshold, int (* chash)( const KEY& a ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<KEY> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<KEY> )
            hash = to_copy.hash;//throw TemplateFunctionError("ArrayPriorityQueue::copy constructor: neither specified");
        if ( thash != ( hashfunc ) undefinedhash<KEY> && chash != ( hashfunc ) undefinedhash<KEY> && thash != chash )
            throw TemplateFunctionError( "HashOpenMap::copy constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenMap::copy constructor: load_threshold is > 1" );

        if ( hash == to_copy.hash )
        {
            bins = to_copy.bins;
            used = to_copy.used;

            map = new Entry[bins];
            state = new bin_state[bins]();

            for ( int i = 0; i < bins; ++i )
            {
                map[ i ] = to_copy.map[ i ];
                state[ i ] = to_copy.state[ i ];
            }
        }
        else
        {
            // start with size 1 arrays in default case (avoids nullptr checks)
            map = new Entry[1];
            state = new bin_state[1]();

            put_all( to_copy );
        }
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::HashOpenMap( const std::initializer_list<Entry>& il, double the_load_threshold, int (* chash)( const KEY& k ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<KEY> ? thash : chash )
    {
        if ( hash != ( hashfunc ) undefinedhash<KEY> && chash != ( hashfunc ) undefinedhash<KEY> && thash != chash )
            throw TemplateFunctionError( "HashOpenMap::initializer_list constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenMap::initializer_list constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        map = new Entry[1];
        state = new bin_state[1]();

        for ( const Entry& entry : il )
        {
            this->put( entry.first, entry.second );
        }
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    template< class Iterable >
    HashOpenMap<KEY, T, thash>::HashOpenMap( const Iterable& i, double the_load_threshold, int (* chash)( const KEY& k ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<KEY> ? thash : chash )
    {
        if ( hash != ( hashfunc ) undefinedhash<KEY> && chash != ( hashfunc ) undefinedhash<KEY> && thash != chash )
            throw TemplateFunctionError( "HashOpenMap::Iterable constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenMap::Iterable constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        map = new Entry[1];
        state = new bin_state[1]();
        for ( const Entry& entry : i )
        {
            this->put( entry.first, entry.second );
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::empty() const
    {
        return used == 0;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    int HashOpenMap<KEY, T, thash>::size() const
    {
        return used;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::has_key( const KEY& key ) const
    {
        return find_key( key ) != -1;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::has_value( const T& value ) const
    {
        if ( used == 0 )
        {
            return false;
        }

        for ( int i = 0; i < bins; ++i )
        {
            if ( state[ i ] == bin_state::bs_occupied )
            {
                if ( map[ i ].second == value )
                {
                    return true;
                }
            }
        }
        return false;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    std::string HashOpenMap<KEY, T, thash>::str() const
    {
        std::ostringstream answer;
        answer << "HashOpenMap[";

        if ( bins != 0 )
        {
            for ( int i = 1; i < bins; ++i )
            {
                answer << "\n" << "bin[" << i << "] = ";

                switch ( state[ i ] )
                {
                    case bin_state::bs_occupied:
                        answer << "occupied : " << map[ i ].first << "->" << map[ i ].second;
                        break;
                    case bin_state::bs_empty:
                        answer << "empty";
                        break;
                    case bin_state::bs_was_occupied:
                        answer << "was occupied : ";
                        break;
                }

            }
        }

        answer << "\n](load_threshold=" << load_threshold << ",bins=" << bins << ",used=" << used << ",mod_count=" << mod_count;
        return answer.str();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    T HashOpenMap<KEY, T, thash>::put( const KEY& key, const T& value )
    {
        ensure_load_threshold( used + 1 );
        ++mod_count;

        int existingIndex = find_key( key );
        if ( existingIndex != -1 )
        {
            // found in the map
            T oldValue = map[ existingIndex ].second;
            map[ existingIndex ].second = value;
            return oldValue;
        }

        // not in the map
        ++used;
        int hashIndex = hash_compress( key );
        int putIndex = next_unoccupied( hashIndex );
        map[ putIndex ].first = key;
        map[ putIndex ].second = value;
        state[ putIndex ] = bin_state::bs_occupied;


        return map[ putIndex ].second;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    T HashOpenMap<KEY, T, thash>::erase( const KEY& key )
    {
        int index = find_key( key );
        if ( index == -1 )
        {
            std::ostringstream error;
            error << "HashOpenMap::erase: key(" << key << ") not in Map";
            throw KeyError( error.str());
        }
        --used;
        ++mod_count;
        state[ index ] = bin_state::bs_was_occupied;
        return map[ index ].second;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    void HashOpenMap<KEY, T, thash>::clear()
    {
        delete[] map;
        delete[] state;

        map = new Entry[1];
        state = new bin_state[1]();

        used = 0;
        bins = 1;
        ++mod_count;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    template< class Iterable >
    int HashOpenMap<KEY, T, thash>::put_all( const Iterable& i )
    {
        int count = 0;
        for ( const Entry& entry : i )
        {
            ++count;
            this->put( entry.first, entry.second );
        }
        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    T& HashOpenMap<KEY, T, thash>::operator[]( const KEY& key )
    {
        int existingIndex = find_key( key );
        if ( existingIndex != -1 )
        {
            // found in the map
            return map[ existingIndex ].second;
        }

        // not in the map
        ensure_load_threshold( used + 1 );
        ++used;
        ++mod_count;
        int hashIndex = hash_compress( key );
        int putIndex = next_unoccupied( hashIndex );
        map[ putIndex ].first = key;
        map[ putIndex ].second = T{};
        state[ putIndex ] = bin_state::bs_occupied;
        return map[ putIndex ].second;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    const T& HashOpenMap<KEY, T, thash>::operator[]( const KEY& key ) const
    {
        int existingIndex = find_key( key );
        if ( existingIndex != -1 )
        {
            // found in the map
            return map[ existingIndex ].second;
        }

        // not in the map
        std::ostringstream error;
        error << "HashOpenMap::operator[]: key(" << key << ") not in Map";
        throw KeyError( error.str());
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>& HashOpenMap<KEY, T, thash>::operator=( const HashOpenMap<KEY, T, thash>& rhs )
    {
        if ( this == &rhs )
            return *this;

        if ( hash == rhs.hash )
        {
            bins = rhs.bins;
            used = rhs.used;

            map = new Entry[bins];
            state = new bin_state[bins]();

            for ( int i = 0; i < bins; ++i )
            {
                map[ i ] = rhs.map[ i ];
                state[ i ] = rhs.state[ i ];
            }
        }
        else
        {
            put_all( rhs );
        }
        ++mod_count;
        return *this;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::operator==( const HashOpenMap<KEY, T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return true;

        if ( used != rhs.size())
            return false;

        for ( auto it = this->begin(); it != this->end(); ++it )
        {
            if ( !rhs.has_key( it->first ) || rhs[ it->first ] != it->second )
            {
                return false;
            }
        }

        return true;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::operator!=( const HashOpenMap<KEY, T, thash>& rhs ) const
    {
        return !( *this == rhs );
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    std::ostream& operator<<( std::ostream& outs, const HashOpenMap<KEY, T, thash>& m )
    {
        outs << "map[";

        if ( !m.empty())
        {
            auto it = m.begin(); // shouldn't be end
            outs << it->first << "->" << it->second;
            ++it;
            for ( ; it != m.end(); ++it )
            {
                outs << "," << it->first << "->" << it->second;
            }
        }

        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    auto HashOpenMap<KEY, T, thash>::begin() const -> HashOpenMap<KEY, T, thash>::Iterator
    {
        return Iterator( const_cast<HashOpenMap <KEY, T, thash>*>(this), 0 );
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    auto HashOpenMap<KEY, T, thash>::end() const -> HashOpenMap<KEY, T, thash>::Iterator
    {
        return Iterator( const_cast<HashOpenMap <KEY, T, thash>*>(this), bins );
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    int HashOpenMap<KEY, T, thash>::hash_compress( const KEY& key ) const
    {
        return std::abs( this->hash( key )) % bins;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    int HashOpenMap<KEY, T, thash>::find_key( const KEY& key ) const
    {
        int hashIndex = hash_compress( key );
        for ( int i = hashIndex, j = 0; j < bins; ++j ) // search all bins once
        {
            if ( state[ i ] == bin_state::bs_occupied && map[ i ].first == key )
            {
                return i;
            }
            else if ( state[ i ] == bin_state::bs_empty )
            {
                return -1;
            }


            if ( ++i == this->bins )
            {
                i = 0;
            }
        }

        return -1;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    int HashOpenMap<KEY, T, thash>::next_unoccupied( int bin ) const
    {
        for ( int i = bin, j = 0; j < bins; ++j ) // search through all bins once
        {
            if ( state[ i ] != bin_state::bs_occupied )
            {
                return i;
            }

            if ( ++i == this->bins )
            {
                i = 0;
            }
        }
        return -1;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    void HashOpenMap<KEY, T, thash>::ensure_load_threshold( int new_used )
    {
        double new_load_factor = static_cast<double>(new_used) / bins;
        if ( new_load_factor > load_threshold )
        {
            ++mod_count;

            Entry* oldMap = map;
            bin_state* oldState = state;
            bins *= 2;
            map = new Entry[bins];
            state = new bin_state[bins]();
            for ( int i = 0; i < bins / 2; ++i )
            {
                if ( oldState[ i ] == bin_state::bs_occupied )
                {
                    int hashIndex = hash_compress( oldMap[ i ].first );
                    int insertIndex = next_unoccupied( hashIndex );
                    map[ insertIndex ].first = oldMap[ i ].first;
                    map[ insertIndex ].second = oldMap[ i ].second;
                    state[ insertIndex ] = bin_state::bs_occupied;
                }
            }

            delete[] oldMap;
            delete[] oldState;


        }
    }






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::Iterator::Iterator( HashOpenMap<KEY, T, thash>* iterate_over, int initial )
            : ref_map( iterate_over ), expected_mod_count( iterate_over->mod_count ), current( initial )
    {
        while ( current < ref_map->bins && ref_map->state[ current ] != bs_occupied )
        {
            ++current;
        }
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    HashOpenMap<KEY, T, thash>::Iterator::~Iterator()
    {
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    auto HashOpenMap<KEY, T, thash>::Iterator::erase() -> Entry
    {
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::erase" );
        if ( !can_erase )
            throw CannotEraseError( "HashOpenMap::Iterator::erase Iterator cursor already erased" );
        if ( current < 0 || current >= ref_map->bins )
            throw CannotEraseError( "HashOpenMap::Iterator::erase Iterator cursor beyond data structure" );

        can_erase = false;

        Entry to_return = ref_map->map[ current ]; // get erased value
        ref_map->state[ current ] = bin_state::bs_was_occupied; // ensure bin is now unoccupied

        // move cursor to next valid value
        while ( ++current < ref_map->bins && ref_map->state[ current ] != bs_occupied )
        {
        }

        --ref_map->used;
        ++ref_map->mod_count;
        expected_mod_count = ref_map->mod_count;
        return to_return;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    std::string HashOpenMap<KEY, T, thash>::Iterator::str() const
    {
        std::ostringstream answer;
        answer << ref_map->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    auto HashOpenMap<KEY, T, thash>::Iterator::operator++() -> HashOpenMap<KEY, T, thash>::Iterator&
    {
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator ++" );

        if ( current >= ref_map->bins )
            return *this;

        if ( can_erase )
        {
            while ( ++current < ref_map->bins && ref_map->state[ current ] != bs_occupied )
            {
            }
        }
        else
            can_erase = true;

        return *this;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    auto HashOpenMap<KEY, T, thash>::Iterator::operator++( int ) -> HashOpenMap<KEY, T, thash>::Iterator
    {
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator ++(int)" );

        if ( current >= ref_map->bins )
            return *this;

        Iterator to_return( *this );
        if ( can_erase )
        {
            while ( ++current < ref_map->bins && ref_map->state[ current ] != bs_occupied )
            {
            }
        }
        else
            can_erase = true;

        return to_return;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::Iterator::operator==( const HashOpenMap<KEY, T, thash>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "HashOpenMap::Iterator::operator ==" );
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator ==" );
        if ( ref_map != rhsASI->ref_map )
            throw ComparingDifferentIteratorsError( "HashOpenMap::Iterator::operator ==" );

        return current == rhsASI->current;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    bool HashOpenMap<KEY, T, thash>::Iterator::operator!=( const HashOpenMap<KEY, T, thash>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "HashOpenMap::Iterator::operator !=" );
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator !=" );
        if ( ref_map != rhsASI->ref_map )
            throw ComparingDifferentIteratorsError( "HashOpenMap::Iterator::operator !=" );

        return current != rhsASI->current;
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    pair<KEY, T>& HashOpenMap<KEY, T, thash>::Iterator::operator*() const
    {
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator *" );
        if ( !can_erase || current < 0 || current >= ref_map->bins )
        {
            std::ostringstream where;
            where << current << " when size = " << ref_map->size();
            throw IteratorPositionIllegal( "HashOpenMap::Iterator::operator * Iterator illegal: " + where.str());
        }

        return ref_map->map[ current ];
    }


    template< class KEY, class T, int (* thash)( const KEY& a ) >
    pair<KEY, T>* HashOpenMap<KEY, T, thash>::Iterator::operator->() const
    {
        if ( expected_mod_count != ref_map->mod_count )
            throw ConcurrentModificationError( "HashOpenMap::Iterator::operator ->" );
        if ( !can_erase || current < 0 || current >= ref_map->bins )
        {
            std::ostringstream where;
            where << current << " when size = " << ref_map->size();
            throw IteratorPositionIllegal( "HashOpenMap::Iterator::operator -> Iterator illegal: " + where.str());
        }

        return &( ref_map->map[ current ] );
    }


}

#endif /* HASH_OPEN_MAP_HPP_ */
