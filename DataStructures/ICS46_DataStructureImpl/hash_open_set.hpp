#ifndef HASH_OPEN_SET_HPP_
#define HASH_OPEN_SET_HPP_

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
    template< class T, int (* thash)( const T& a ) = undefinedhash<T>>
    class HashOpenSet
    {
    public:
        typedef int (* hashfunc)( const T& a );

        //Destructor/Constructors
        ~HashOpenSet();

        HashOpenSet( double the_load_threshold = 1.0, int (* chash)( const T& a ) = undefinedhash<T> );

        explicit HashOpenSet( int initial_bins, double the_load_threshold = 1.0, int (* chash)( const T& k ) = undefinedhash<T> );

        HashOpenSet( const HashOpenSet<T, thash>& to_copy, double the_load_threshold = 1.0, int (* chash)( const T& a ) = undefinedhash<T> );

        explicit HashOpenSet( const std::initializer_list<T>& il, double the_load_threshold = 1.0, int (* chash)( const T& a ) = undefinedhash<T> );

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        explicit
        HashOpenSet( const Iterable& i, double the_load_threshold = 1.0, int (* chash)( const T& a ) = undefinedhash<T> );


        //Queries
        bool empty() const;

        int size() const;

        bool contains( const T& element ) const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        bool contains_all( const Iterable& i ) const;


        //Commands
        int insert( const T& element );

        int erase( const T& element );

        void clear();

        //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

        template< class Iterable >
        int insert_all( const Iterable& i );

        template< class Iterable >
        int erase_all( const Iterable& i );

        template< class Iterable >
        int retain_all( const Iterable& i );


        //Operators
        HashOpenSet<T, thash>& operator=( const HashOpenSet<T, thash>& rhs );

        bool operator==( const HashOpenSet<T, thash>& rhs ) const;

        bool operator!=( const HashOpenSet<T, thash>& rhs ) const;

        bool operator<=( const HashOpenSet<T, thash>& rhs ) const;

        bool operator<( const HashOpenSet<T, thash>& rhs ) const;

        bool operator>=( const HashOpenSet<T, thash>& rhs ) const;

        bool operator>( const HashOpenSet<T, thash>& rhs ) const;

        template< class T2, int (* hash2)( const T2& a ) >
        friend std::ostream& operator<<( std::ostream& outs, const HashOpenSet<T2, hash2>& s );


    private:
        class LN;

    public:
        class Iterator
        {
        public:
            //Private constructor called in begin/end, which are friends of HashOpenSet<T,thash>
            ~Iterator();

            T erase();

            std::string str() const;

            HashOpenSet<T, thash>::Iterator& operator++();

            HashOpenSet<T, thash>::Iterator operator++( int );

            bool operator==( const HashOpenSet<T, thash>::Iterator& rhs ) const;

            bool operator!=( const HashOpenSet<T, thash>::Iterator& rhs ) const;

            T& operator*() const;

            T* operator->() const;

            friend std::ostream& operator<<( std::ostream& outs, const HashOpenSet<T, thash>::Iterator& i )
            {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }

            friend Iterator HashOpenSet<T, thash>::begin() const;

            friend Iterator HashOpenSet<T, thash>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            int current; //Bin Index
            HashOpenSet<T, thash>* ref_set;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator( HashOpenSet<T, thash>* iterate_over, int initial );
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        int (* hash)( const T& k );      //Hashing function used (from template or constructor)
        enum bin_state
        {
            bs_empty, bs_occupied, bs_was_occupied
        };
        T* set = nullptr;    //T array
        bin_state* state = nullptr;    //bin_state[] describes the state of set[i]
        double load_threshold;         //used/bins <= load_threshold
        int bins = 1;          //# bins in array (should start at 1 so hash_compress doesn't % 0)
        int used = 0;          //Cache for number of elements in the hash table
        int mod_count = 0;          //For sensing concurrent modification


        //Helper methods
        int hash_compress( const T& element ) const;  //hash function ranged to [0,bins-1]
        int find_element( const T& kelement ) const;  //Returns index to element's bin or -1
        int next_unoccupied( int bin ) const;  //this bin or next beyond that is unoccupied

        void ensure_load_threshold( int new_used );           //Reallocate if load_factor > load_threshold
    };





//HashOpenSet class and related definitions

////////////////////////////////////////////////////////////////////////////////
//
//Destructor/Constructors

    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::~HashOpenSet()
    {
        this->clear();
    }


    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::HashOpenSet( double the_load_threshold, int (* chash)( const T& element ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<T> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<T> )
            throw TemplateFunctionError( "HashOpenSet::default constructor: neither specified" );
        if ( thash != ( hashfunc ) undefinedhash<T> && chash != ( hashfunc ) undefinedhash<T> && thash != chash )
            throw TemplateFunctionError( "HashOpenSet::default constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenSet::default constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        set = new T[1];
        state = new bin_state[1]();
    }


    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::HashOpenSet( int initial_bins, double the_load_threshold, int (* chash)( const T& element ))
            : bins( initial_bins ), load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<T> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<T> )
            throw TemplateFunctionError( "HashOpenSet::initial_bins constructor: neither specified" );
        if ( thash != ( hashfunc ) undefinedhash<T> && chash != ( hashfunc ) undefinedhash<T> && thash != chash )
            throw TemplateFunctionError( "HashOpenSet::initial_bins constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenSet::initial_bins constructor: load_threshold is > 1" );

        set = new T[initial_bins];
        state = new bin_state[initial_bins]();
    }

    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::HashOpenSet( const HashOpenSet<T, thash>& to_copy, double the_load_threshold, int (* chash)( const T& element ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<T> ? thash : chash )
    {
        if ( hash == ( hashfunc ) undefinedhash<T> )
            hash = to_copy.hash;//throw TemplateFunctionError("ArrayPriorityQueue::copy constructor: neither specified");
        if ( thash != ( hashfunc ) undefinedhash<T> && chash != ( hashfunc ) undefinedhash<T> && thash != chash )
            throw TemplateFunctionError( "HashOpenSet::copy constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenSet::copy constructor: load_threshold is > 1" );

        if ( hash == to_copy.hash )
        {
            bins = to_copy.bins;
            used = to_copy.used;

            set = new T[bins];
            state = new bin_state[bins]();

            for ( int i = 0; i < bins; ++i )
            {
                set[ i ] = to_copy.set[ i ];
                state[ i ] = to_copy.state[ i ];
            }
        }
        else
        {
            // start with size 1 arrays in default case (avoids nullptr checks)
            set = new T[1];
            state = new bin_state[1]();

            insert_all( to_copy );
        }
    }


    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::HashOpenSet( const std::initializer_list<T>& il, double the_load_threshold, int (* chash)( const T& element ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<T> ? thash : chash )
    {
        if ( hash != ( hashfunc ) undefinedhash<T> && chash != ( hashfunc ) undefinedhash<T> && thash != chash )
            throw TemplateFunctionError( "HashOpenSet::initializer_list constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenSet::initializer_list constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        set = new T[1];
        state = new bin_state[1]();

        for ( const T& v : il )
        {
            this->insert( v );
        }
    }

    template< class T, int (* thash)( const T& a ) >
    template< class Iterable >
    HashOpenSet<T, thash>::HashOpenSet( const Iterable& i, double the_load_threshold, int (* chash)( const T& a ))
            : load_threshold( the_load_threshold ), hash( thash != ( hashfunc ) undefinedhash<T> ? thash : chash )
    {
        if ( hash != ( hashfunc ) undefinedhash<T> && chash != ( hashfunc ) undefinedhash<T> && thash != chash )
            throw TemplateFunctionError( "HashOpenSet::Iterable constructor: both specified and different" );
        if ( load_threshold > 1 )
            throw IcsError( "HashOpenSet::Iterable constructor: load_threshold is > 1" );

        // start with size 1 arrays in default case (avoids nullptr checks)
        set = new T[1];
        state = new bin_state[1]();
        for ( const T& v : i )
        {
            this->insert( v );
        }
    }
////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::empty() const
    {
        return used == 0;
    }


    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::size() const
    {
        return used;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::contains( const T& element ) const
    {
        return find_element( element ) != -1;
    }


    template< class T, int (* thash)( const T& a ) >
    std::string HashOpenSet<T, thash>::str() const
    {
        std::ostringstream answer;
        answer << "HashOpenSet[";

        if ( bins != 0 )
        {
            for ( int i = 1; i < bins; ++i )
            {
                answer << "\n" << "bin[" << i << "] = ";

                switch ( state[ i ] )
                {
                    case bin_state::bs_occupied:
                        answer << "occupied : " << set[ i ];
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


    template< class T, int (* thash)( const T& a ) >
    template< class Iterable >
    bool HashOpenSet<T, thash>::contains_all( const Iterable& i ) const
    {
        for ( const T& v : i )
        {
            if ( find_element( v ) == -1 )
            {
                return false;
            }
        }
        return true;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::insert( const T& element )
    {
        ensure_load_threshold( used + 1 );

        int existingIndex = find_element( element );
        if ( existingIndex != -1 )
        {
            // found in the set
            return 0;
        }

        // not in the set
        ++used;
        ++mod_count;
        int hashIndex = hash_compress( element );
        int putIndex = next_unoccupied( hashIndex );
        set[ putIndex ] = element;
        state[ putIndex ] = bin_state::bs_occupied;


        return 1;
    }


    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::erase( const T& element )
    {
        int index = find_element( element );
        if ( index == -1 )
        {
            return 0;
        }
        --used;
        ++mod_count;
        state[ index ] = bin_state::bs_was_occupied;
        return 1;
    }


    template< class T, int (* thash)( const T& a ) >
    void HashOpenSet<T, thash>::clear()
    {
        delete[] set;
        delete[] state;

        set = new T[1];
        state = new bin_state[1]();

        used = 0;
        bins = 1;
        ++mod_count;
    }


    template< class T, int (* thash)( const T& a ) >
    template< class Iterable >
    int HashOpenSet<T, thash>::insert_all( const Iterable& i )
    {
        int count = 0;
        for ( const T& v : i )
        {
            count += this->insert( v );
        }
        return count;
    }


    template< class T, int (* thash)( const T& a ) >
    template< class Iterable >
    int HashOpenSet<T, thash>::erase_all( const Iterable& i )
    {
        int count = 0;
        for ( const T& v : i )
        {
            if ( this->contains( v ))
            {
                count += erase( v );
            }
        }
        return count;
    }


    template< class T, int (* thash)( const T& a ) >
    template< class Iterable >
    int HashOpenSet<T, thash>::retain_all( const Iterable& i )
    {
        HashOpenSet s( i );
        int count = 0;
        for ( auto it = this->begin(); it != this->end(); ++it)
        {
            if ( !s.contains( *it ))
            {
                it.erase();
                ++count;
            }
        }

        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>& HashOpenSet<T, thash>::operator=( const HashOpenSet<T, thash>& rhs )
    {
        if ( this == &rhs )
            return *this;

        if ( hash == rhs.hash )
        {
            bins = rhs.bins;
            used = rhs.used;

            set = new T[bins];
            state = new bin_state[bins]();

            for ( int i = 0; i < bins; ++i )
            {
                set[ i ] = rhs.set[ i ];
                state[ i ] = rhs.state[ i ];
            }
        }
        else
        {
            insert_all( rhs );
        }
        ++mod_count;
        return *this;
    }

    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator==( const HashOpenSet<T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return true;

        if ( used != rhs.size())
            return false;

        for ( auto it = this->begin(); it != this->end(); ++it )
        {
            if ( !rhs.contains( *it ))
            {
                return false;
            }
        }

        return true;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator!=( const HashOpenSet<T, thash>& rhs ) const
    {
        return !( *this == rhs );
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator<=( const HashOpenSet<T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return true;

        if ( used > rhs.size())
            return false;

        for ( auto it = this->begin(); it != this->end(); ++it )
        {
            if ( !rhs.contains( *it ))
                return false;
        }

        return true;
    }

    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator<( const HashOpenSet<T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return false;

        if ( used >= rhs.size())
            return false;

        for ( auto it = this->begin(); it != this->end(); ++it )
        {
            if ( !rhs.contains( *it ))
                return false;
        }

        return true;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator>=( const HashOpenSet<T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return true;

        if ( rhs.size() > used )
            return false;

        for ( auto it = rhs.begin(); it != rhs.end(); ++it )
        {
            if ( !this->contains( *it ))
                return false;
        }

        return true;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::operator>( const HashOpenSet<T, thash>& rhs ) const
    {
        if ( this == &rhs )
            return true;

        if ( rhs.size() >= used )
            return false;

        for ( auto it = rhs.begin(); it != rhs.end(); ++it )
        {
            if ( !this->contains( *it ))
                return false;
        }

        return true;
    }


    template< class T, int (* thash)( const T& a ) >
    std::ostream& operator<<( std::ostream& outs, const HashOpenSet<T, thash>& s )
    {
        outs << "set[";

        if ( !s.empty())
        {
            auto it = s.begin(); // shouldn't be end
            outs << *it;
            ++it;
            for ( ; it != s.end(); ++it )
            {
                outs << "," << *it;
            }
        }

        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template< class T, int (* thash)( const T& a ) >
    auto HashOpenSet<T, thash>::begin() const -> HashOpenSet<T, thash>::Iterator
    {
        return Iterator( const_cast<HashOpenSet <T, thash>*>(this), 0 );
    }


    template< class T, int (* thash)( const T& a ) >
    auto HashOpenSet<T, thash>::end() const -> HashOpenSet<T, thash>::Iterator
    {
        return Iterator( const_cast<HashOpenSet <T, thash>*>(this), bins );
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::hash_compress( const T& element ) const
    {
        return std::abs( this->hash( element )) % bins;
    }


    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::find_element( const T& element ) const
    {
        int hashIndex = hash_compress( element );
        for ( int i = hashIndex, j = 0; j < bins; ++j ) // search all bins once
        {
            if ( state[ i ] == bin_state::bs_occupied && set[ i ] == element )
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


    template< class T, int (* thash)( const T& a ) >
    int HashOpenSet<T, thash>::next_unoccupied( int bin ) const
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


    template< class T, int (* thash)( const T& a ) >
    void HashOpenSet<T, thash>::ensure_load_threshold( int new_used )
    {
        double new_load_factor = static_cast<double>(new_used) / bins;
        if ( new_load_factor > load_threshold )
        {
            ++mod_count;

            T* oldSet = set;
            bin_state* oldState = state;
            bins *= 2;
            set = new T[bins];
            state = new bin_state[bins]();
            for ( int i = 0; i < bins / 2; ++i )
            {
                if ( oldState[ i ] == bin_state::bs_occupied )
                {
                    int hashIndex = hash_compress( oldSet[ i ] );
                    int insertIndex = next_unoccupied( hashIndex );
                    set[ insertIndex ] = oldSet[ i ];
                    state[ insertIndex ] = bin_state::bs_occupied;
                }
            }

            delete[] oldSet;
            delete[] oldState;


        }
    }






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::Iterator::Iterator( HashOpenSet<T, thash>* iterate_over, int initial )
            : ref_set( iterate_over ), expected_mod_count( iterate_over->mod_count ), current( initial )
    {
        while ( current < ref_set->bins && ref_set->state[ current ] != bs_occupied )
        {
            ++current;
        }
    }


    template< class T, int (* thash)( const T& a ) >
    HashOpenSet<T, thash>::Iterator::~Iterator()
    {
    }


    template< class T, int (* thash)( const T& a ) >
    T HashOpenSet<T, thash>::Iterator::erase()
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::erase" );
        if ( !can_erase )
            throw CannotEraseError( "HashOpenSet::Iterator::erase Iterator cursor already erased" );
        if ( current < 0 || current >= ref_set->bins )
            throw CannotEraseError( "HashOpenSet::Iterator::erase Iterator cursor beyond data structure" );

        can_erase = false;

        T to_return = ref_set->set[ current ]; // get erased value
        ref_set->erase( to_return );

        // move cursor to next valid value
        while ( ++current < ref_set->bins && ref_set->state[ current ] != bs_occupied )
        {
        }

        expected_mod_count = ref_set->mod_count;
        return to_return;
    }


    template< class T, int (* thash)( const T& a ) >
    std::string HashOpenSet<T, thash>::Iterator::str() const
    {
        std::ostringstream answer;
        answer << ref_set->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }


    template< class T, int (* thash)( const T& a ) >
    auto HashOpenSet<T, thash>::Iterator::operator++() -> HashOpenSet<T, thash>::Iterator&
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator ++" );

        if ( current >= ref_set->bins )
            return *this;

        if ( can_erase )
        {
            while ( ++current < ref_set->bins && ref_set->state[ current ] != bs_occupied )
            {
            }
        }
        else
            can_erase = true;

        return *this;
    }


    template< class T, int (* thash)( const T& a ) >
    auto HashOpenSet<T, thash>::Iterator::operator++( int ) -> HashOpenSet<T, thash>::Iterator
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator ++(int)" );

        if ( current >= ref_set->bins )
            return *this;

        Iterator to_return( *this );
        if ( can_erase )
        {
            while ( ++current < ref_set->bins && ref_set->state[ current ] != bs_occupied )
            {
            }
        }
        else
            can_erase = true;

        return to_return;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::Iterator::operator==( const HashOpenSet<T, thash>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "HashOpenSet::Iterator::operator ==" );
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator ==" );
        if ( ref_set != rhsASI->ref_set )
            throw ComparingDifferentIteratorsError( "HashOpenSet::Iterator::operator ==" );

        return current == rhsASI->current;
    }


    template< class T, int (* thash)( const T& a ) >
    bool HashOpenSet<T, thash>::Iterator::operator!=( const HashOpenSet<T, thash>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "HashOpenSet::Iterator::operator !=" );
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator !=" );
        if ( ref_set != rhsASI->ref_set )
            throw ComparingDifferentIteratorsError( "HashOpenSet::Iterator::operator !=" );

        return current != rhsASI->current;
    }

    template< class T, int (* thash)( const T& a ) >
    T& HashOpenSet<T, thash>::Iterator::operator*() const
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator *" );
        if ( !can_erase || current < 0 || current >= ref_set->bins )
        {
            std::ostringstream where;
            where << current << " when size = " << ref_set->size();
            throw IteratorPositionIllegal( "HashOpenSet::Iterator::operator * Iterator illegal: " + where.str());
        }

        return ref_set->set[ current ];
    }

    template< class T, int (* thash)( const T& a ) >
    T* HashOpenSet<T, thash>::Iterator::operator->() const
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "HashOpenSet::Iterator::operator ->" );
        if ( !can_erase || current < 0 || current >= ref_set->bins )
        {
            std::ostringstream where;
            where << current << " when size = " << ref_set->size();
            throw IteratorPositionIllegal( "HashOpenSet::Iterator::operator -> Iterator illegal: " + where.str());
        }

        return &( ref_set->set[ current ] );
    }

}

#endif /* HASH_OPEN_SET_HPP_ */
