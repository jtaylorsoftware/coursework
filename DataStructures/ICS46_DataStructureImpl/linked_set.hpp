#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics
{


    template< class T >
    class LinkedSet
    {
    public:
        //Destructor/Constructors
        ~LinkedSet();

        LinkedSet();

        explicit LinkedSet( int initialLength );

        LinkedSet( const LinkedSet<T>& to_copy );

        explicit LinkedSet( const std::initializer_list<T>& il );

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        explicit LinkedSet( const Iterable& i );


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
        LinkedSet<T>& operator=( const LinkedSet<T>& rhs );

        bool operator==( const LinkedSet<T>& rhs ) const;

        bool operator!=( const LinkedSet<T>& rhs ) const;

        bool operator<=( const LinkedSet<T>& rhs ) const;

        bool operator<( const LinkedSet<T>& rhs ) const;

        bool operator>=( const LinkedSet<T>& rhs ) const;

        bool operator>( const LinkedSet<T>& rhs ) const;

        template< class T2 >
        friend std::ostream& operator<<( std::ostream& outs, const LinkedSet<T2>& s );


    private:
        class LN;

    public:
        class Iterator
        {
        public:
            //Private constructor called in begin/end, which are friends of LinkedSet<T>
            ~Iterator();

            T erase();

            std::string str() const;

            LinkedSet<T>::Iterator& operator++();

            LinkedSet<T>::Iterator operator++( int );

            bool operator==( const LinkedSet<T>::Iterator& rhs ) const;

            bool operator!=( const LinkedSet<T>::Iterator& rhs ) const;

            T& operator*() const;

            T* operator->() const;

            friend std::ostream& operator<<( std::ostream& outs, const LinkedSet<T>::Iterator& i )
            {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }

            friend Iterator LinkedSet<T>::begin() const;

            friend Iterator LinkedSet<T>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            LN* current;  //if can_erase is false, this value is unusable
            LinkedSet<T>* ref_set;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator( LinkedSet<T>* iterate_over, LN* initial );
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        class LN
        {
        public:
            LN()
            {
            }

            LN( const LN& ln ) : value( ln.value ), next( ln.next )
            {
            }

            LN( T v, LN* n = nullptr ) : value( v ), next( n )
            {
            }

            T value;
            LN* next = nullptr;
        };


        LN* front = new LN();
        LN* trailer = front;         //Invariant: point to the special trailer node
        int used = 0;            //Cache count for nodes in linked list
        int mod_count = 0;             //For sensing concurrent modification

        //Helper methods
        int erase_at( LN* p );

        void delete_list( LN*& front );  //Deallocate all LNs (but trailer), and set front's argument to trailer;
    };





////////////////////////////////////////////////////////////////////////////////
//
//LinkedSet class and related definitions

//Destructor/Constructors

    template< class T >
    LinkedSet<T>::~LinkedSet()
    {
        delete_list( front );
    }


    template< class T >
    LinkedSet<T>::LinkedSet()
    {
    }


    template< class T >
    LinkedSet<T>::LinkedSet( const LinkedSet<T>& to_copy ) : used( to_copy.used )
    {
        for ( const auto& v : to_copy )
        {
            front = new LN( v, front );
        }
    }


    template< class T >
    LinkedSet<T>::LinkedSet( const std::initializer_list<T>& il )
    {
        for ( const auto& v : il )
        {
            insert( v );
        }
    }


    template< class T >
    template< class Iterable >
    LinkedSet<T>::LinkedSet( const Iterable& i )
    {
        for ( const auto& v: i )
        {
            insert( v );
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template< class T >
    bool LinkedSet<T>::empty() const
    {
        return used == 0;
    }


    template< class T >
    int LinkedSet<T>::size() const
    {
        return used;
    }


    template< class T >
    bool LinkedSet<T>::contains( const T& element ) const
    {
        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( ln->value == element )
            {
                return true;
            }
        }
        return false;
    }


    template< class T >
    std::string LinkedSet<T>::str() const
    {
        std::ostringstream result;
        result << "LinkedSet[";

        if ( used != 0 )
        {
            int count = 0;
            for ( auto* ln = front; ln != trailer; ln = ln->next )
            {
                result << count++ << ":" << ln->value << ( ln->next == trailer ? "]" : "," );
            }
        }
        else
        {
            result << "]";
        }

        result << "(length=" << used << ",used=" << used << ",mod_count=" << mod_count << ")";
        return result.str();
    }


    template< class T >
    template< class Iterable >
    bool LinkedSet<T>::contains_all( const Iterable& i ) const
    {
        for ( const auto& v : i )
        {
            if ( !this->contains( v ))
            {
                return false;
            }
        }

        return true;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands


    template< class T >
    int LinkedSet<T>::insert( const T& element )
    {
        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( ln->value == element )
            {
                return 0;
            }
        }

        // insert at front (and move front over)
        front = new LN( element, front );

        ++used;
        ++mod_count;
        return 1;
    }


    template< class T >
    int LinkedSet<T>::erase( const T& element )
    {
        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( ln->value == element )
            {
                return erase_at( ln );
            }
        }

        return 0;
    }


    template< class T >
    void LinkedSet<T>::clear()
    {
        delete_list( front );
        used = 0;
        ++mod_count;
    }


    template< class T >
    template< class Iterable >
    int LinkedSet<T>::insert_all( const Iterable& i )
    {
        int count = 0;
        for ( const auto& v : i )
        {
            count += insert( v );
        }

        return count;
    }


    template< class T >
    template< class Iterable >
    int LinkedSet<T>::erase_all( const Iterable& i )
    {
        int count = 0;
        for ( const auto& v : i )
        {
            count += erase( v );
        }

        return count;
    }


    template< class T >
    template< class Iterable >
    int LinkedSet<T>::retain_all( const Iterable& i )
    {
        LinkedSet retainSet( i );
        int count = 0;

        for ( auto i = this->begin(); i != this->end(); ++i )
        {
            if ( !retainSet.contains( *i ))
            {
                i.erase();
                ++count;
            }
        }

        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template< class T >
    LinkedSet<T>& LinkedSet<T>::operator=( const LinkedSet<T>& rhs )
    {
        if ( this == &rhs )
        {
            return *this;
        }

//        used = rhs.used;
//
//        delete_list( front );
//
//        for ( const T& v : rhs )
//        {
//            front = new LN( v, front );
//        }
        if ( used > rhs.used )
        {
            if ( rhs.empty())
            {
                this->clear();
                return *this;
            }

            auto* lhsLn = this->front;
            for ( auto rhsIter = rhs.begin();; )
            {
                lhsLn->value = *rhsIter++;
                if ( rhsIter != rhs.end() )
                {
                    lhsLn = lhsLn->next;
                }
                else
                {
                    break;
                }
            }
            trailer = nullptr;
            delete_list( lhsLn->next );
            trailer = lhsLn;
        }
        else if ( used < rhs.used )
        {
            auto rhsIter = rhs.begin();

            // Copy the values from rhs until the end of current LHS
            for ( auto* ln = this->front;; ln = ln->next )
            {
                ln->value = *rhsIter++;

                // If end of LHS, stop
                if ( ln->next == nullptr )
                {
                    trailer = ln;
                    break;
                }
            }

            // Get rest of values
            do
            {
                trailer->next = new LN( *rhsIter++ );
                trailer = trailer->next;
            } while ( rhsIter != rhs.end());

        }
        else
        {
            for ( const auto& v : rhs )
            {
                front = new LN( v, front );
            }
        }


        used = rhs.used;
        ++mod_count;
        return *this;
    }


    template< class T >
    bool LinkedSet<T>::operator==( const LinkedSet<T>& rhs ) const
    {
        if ( this == &rhs )
        {
            return true;
        }

        if ( used != rhs.size())
        {
            return false;
        }

        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( !rhs.contains( ln->value ))
            {
                return false;
            }
        }

        return true;
    }


    template< class T >
    bool LinkedSet<T>::operator!=( const LinkedSet<T>& rhs ) const
    {
        return !( *this == rhs );
    }


    template< class T >
    bool LinkedSet<T>::operator<=( const LinkedSet<T>& rhs ) const
    {
        if ( this == &rhs )
        {
            return true;
        }

        if ( used > rhs.size())
        {
            return false;
        }

        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( !rhs.contains( ln->value ))
            {
                return false;
            }
        }

        return true;
    }


    template< class T >
    bool LinkedSet<T>::operator<( const LinkedSet<T>& rhs ) const
    {
        if ( this == &rhs )
        {
            return false;
        }

        if ( used >= rhs.size())
        {
            return false;
        }

        for ( auto* ln = front; ln != trailer; ln = ln->next )
        {
            if ( !rhs.contains( ln->value ))
            {
                return false;
            }
        }
        return true;
    }


    template< class T >
    bool LinkedSet<T>::operator>=( const LinkedSet<T>& rhs ) const
    {
        return rhs <= *this;
    }


    template< class T >
    bool LinkedSet<T>::operator>( const LinkedSet<T>& rhs ) const
    {
        return rhs < *this;
    }


    template< class T >
    std::ostream& operator<<( std::ostream& outs, const LinkedSet<T>& s )
    {
        outs << "set[";

        if ( !s.empty())
        {
            outs << s.front->value;
            for ( auto* ln = s.front->next; ln != s.trailer; ln = ln->next )
            {
                outs << "," << ln->value;
            }
        }

        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template< class T >
    auto LinkedSet<T>::begin() const -> LinkedSet<T>::Iterator
    {
        return Iterator( const_cast<LinkedSet<T>*>(this), front );
    }


    template< class T >
    auto LinkedSet<T>::end() const -> LinkedSet<T>::Iterator
    {
        return Iterator( const_cast<LinkedSet<T>*>(this), trailer );
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template< class T >
    int LinkedSet<T>::erase_at( LN* p )
    {
        // trick from notes
        auto* toDelete = p->next;
        *p = *toDelete;
        delete toDelete;

        if ( p->next == nullptr )
        {
            // p is now the trailer
            trailer = p;
        }
        --used;
        ++mod_count;
        return 1;
    }


    template< class T >
    void LinkedSet<T>::delete_list( LN*& front )
    {
        if ( front == nullptr )
        {
            return;
        }

        while ( front != nullptr && front != trailer )
        {
            auto* next = front->next;
            delete front;
            front = next;
        }

        front = trailer;
    }





////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template< class T >
    LinkedSet<T>::Iterator::Iterator( LinkedSet<T>* iterate_over, LN* initial )
            : ref_set( iterate_over ), current( initial ), expected_mod_count( iterate_over->mod_count )
    {
    }


    template< class T >
    LinkedSet<T>::Iterator::~Iterator()
    {
    }


    template< class T >
    T LinkedSet<T>::Iterator::erase()
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::erase" );
        if ( !can_erase )
            throw CannotEraseError( "LinkedSet::Iterator::erase Iterator cursor already erased" );

        can_erase = false;
        T to_return = current->value;
        ref_set->erase_at( current );
        expected_mod_count = ref_set->mod_count;
        return to_return;
    }


    template< class T >
    std::string LinkedSet<T>::Iterator::str() const
    {
        std::ostringstream answer;
        answer << ref_set->str() << "(current=" << ( current != nullptr ? current->value : "" ) << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
        return answer.str();
    }


    template< class T >
    auto LinkedSet<T>::Iterator::operator++() -> LinkedSet<T>::Iterator&
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator ++" );

        if ( current->next == nullptr ) // cannot advance past trailer
            return *this;

        if ( can_erase )
        {
            current = current->next;
        }
        else
        {
            can_erase = true;
        }

        return *this;
    }


    template< class T >
    auto LinkedSet<T>::Iterator::operator++( int ) -> LinkedSet<T>::Iterator
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator ++(int)" );

        if ( current->next == nullptr ) // cannot advance past trailer
            return *this;

        Iterator to_return( *this );
        if ( can_erase )
        {
            current = current->next;
        }
        else
        {
            can_erase = true;
        }

        return to_return;
    }


    template< class T >
    bool LinkedSet<T>::Iterator::operator==( const LinkedSet<T>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedSet::Iterator::operator ==" );
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator ==" );
        if ( ref_set != rhsASI->ref_set )
            throw ComparingDifferentIteratorsError( "LinkedSet::Iterator::operator ==" );

        return current->next == rhsASI->current->next;
    }


    template< class T >
    bool LinkedSet<T>::Iterator::operator!=( const LinkedSet<T>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedSet::Iterator::operator !=" );
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator !=" );
        if ( ref_set != rhsASI->ref_set )
            throw ComparingDifferentIteratorsError( "LinkedSet::Iterator::operator !=" );

        return current->next != rhsASI->current->next;
    }


    template< class T >
    T& LinkedSet<T>::Iterator::operator*() const
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator *" );
        if ( !can_erase )
        {
            std::ostringstream where;
            where << current->value << " when size = " << ref_set->size();
            throw IteratorPositionIllegal( "LinkedSet::Iterator::operator * Iterator illegal: " + where.str());
        }

        return current->value;
    }


    template< class T >
    T* LinkedSet<T>::Iterator::operator->() const
    {
        if ( expected_mod_count != ref_set->mod_count )
            throw ConcurrentModificationError( "LinkedSet::Iterator::operator ->" );
        if ( !can_erase )
        {
            std::ostringstream where;
            where << current->value << " when size = " << ref_set->size();
            throw IteratorPositionIllegal( "LinkedSet::Iterator::operator -> Iterator illegal: " + where.str());
        }

        return &current->value;
    }


}

#endif /* LINKED_SET_HPP_ */
