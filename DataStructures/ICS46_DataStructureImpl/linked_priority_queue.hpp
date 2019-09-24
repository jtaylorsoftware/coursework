#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "array_stack.hpp"      //See operator <<


namespace ics
{


#ifndef undefinedgtdefined
#define undefinedgtdefined

    template< class T >
    bool undefinedgt( const T& a, const T& b )
    {
        return false;
    }

#endif /* undefinedgtdefined */

//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to undefinedgt in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedgt value supplied by tgt/cgt is stored in the instance variable gt.
    template< class T, bool (* tgt)( const T& a, const T& b ) = undefinedgt<T>>

    class LinkedPriorityQueue
    {
    public:
        using gtfunc = bool ( * )( const T& a, const T& b );

        //Destructor/Constructors
        ~LinkedPriorityQueue();

        LinkedPriorityQueue( bool (* cgt)( const T& a, const T& b ) = undefinedgt<T> );

        LinkedPriorityQueue( const LinkedPriorityQueue<T, tgt>& to_copy,
                             bool (* cgt)( const T& a, const T& b ) = undefinedgt<T> );

        explicit LinkedPriorityQueue( const std::initializer_list<T>& il,
                                      bool (* cgt)( const T& a, const T& b ) = undefinedgt<T> );

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        explicit LinkedPriorityQueue( const Iterable& i, bool (* cgt)( const T& a, const T& b ) = undefinedgt<T> );


        //Queries
        bool empty() const;

        int size() const;

        T& peek() const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<


        //Commands
        int enqueue( const T& element );

        T dequeue();

        void clear();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        int enqueue_all( const Iterable& i );


        //Operators
        LinkedPriorityQueue<T, tgt>& operator=( const LinkedPriorityQueue<T, tgt>& rhs );

        bool operator==( const LinkedPriorityQueue<T, tgt>& rhs ) const;

        bool operator!=( const LinkedPriorityQueue<T, tgt>& rhs ) const;

        template< class T2, bool (* gt2)( const T2& a, const T2& b ) >
        friend std::ostream& operator<<( std::ostream& outs, const LinkedPriorityQueue<T2, gt2>& pq );


    private:
        class LN;

    public:
        class Iterator
        {
        public:
            //Private constructor called in begin/end, which are friends of LinkedPriorityQueue<T,tgt>
            ~Iterator();

            T erase();

            std::string str() const;

            LinkedPriorityQueue<T, tgt>::Iterator& operator++();

            LinkedPriorityQueue<T, tgt>::Iterator operator++( int );

            bool operator==( const LinkedPriorityQueue<T, tgt>::Iterator& rhs ) const;

            bool operator!=( const LinkedPriorityQueue<T, tgt>::Iterator& rhs ) const;

            T& operator*() const;

            T* operator->() const;

            friend std::ostream& operator<<( std::ostream& outs, const LinkedPriorityQueue<T, tgt>::Iterator& i )
            {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }

            friend Iterator LinkedPriorityQueue<T, tgt>::begin() const;

            friend Iterator LinkedPriorityQueue<T, tgt>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            LN* prev;            //initialize prev to the header node
            LN* current;         //current == prev->next
            LinkedPriorityQueue<T, tgt>* ref_pq;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator( LinkedPriorityQueue<T, tgt>* iterate_over, LN* initial );
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


        bool (* gt)( const T& a, const T& b ); // The gt used by enqueue (from template or constructor)
        LN* front = new LN();
        int used = 0;                  //Cache count for nodes in linked list
        int mod_count = 0;                  //Allows sensing for concurrent modification

        //Helper methods
        void delete_list( LN*& front );        //Deallocate all LNs, and set front's argument to nullptr;
    };





////////////////////////////////////////////////////////////////////////////////
//
//LinkedPriorityQueue class and related definitions

//Destructor/Constructors

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::~LinkedPriorityQueue()
    {
        delete_list( front->next );
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::LinkedPriorityQueue( bool (* cgt)( const T& a, const T& b ))
            : gt( tgt != ( gtfunc ) undefinedgt<T> ? tgt : cgt )
    {
        if ( gt == ( gtfunc ) undefinedgt<T> )
            throw TemplateFunctionError( "LinkedPriorityQueue::default constructor: neither specified" );
        if ( tgt != ( gtfunc ) undefinedgt<T> && cgt != ( gtfunc ) undefinedgt<T> && tgt != cgt )
            throw TemplateFunctionError( "LinkedPriorityQueue::default constructor: both specified and different" );


    }

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::LinkedPriorityQueue( const LinkedPriorityQueue<T, tgt>& to_copy, bool (* cgt)( const T& a, const T& b ))
            : gt( tgt != ( gtfunc ) undefinedgt<T> ? tgt : cgt )
    {
        if ( gt == ( gtfunc ) undefinedgt<T> )
            gt = to_copy.gt;
        if ( tgt != ( gtfunc ) undefinedgt<T> && cgt != ( gtfunc ) undefinedgt<T> && tgt != cgt )
            throw TemplateFunctionError( "LinkedPriorityQueue::copy constructor: both specified and different" );

        if ( gt == to_copy.gt )
        {
            used = to_copy.used;
            if ( used > 0 )
            {
                auto toCopyIter = to_copy.begin();
                front->next = new LN( *toCopyIter );
                ++toCopyIter;
                for ( auto* t = front->next; toCopyIter != to_copy.end(); ++toCopyIter, t = t->next )
                {
                    t->next = new LN( *toCopyIter );
                }
            }
        }
        else
        {
            for ( const auto& v : to_copy )
            {
                enqueue( v );
            }
        }

    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::LinkedPriorityQueue( const std::initializer_list<T>& il,
                                                      bool (* cgt)( const T& a, const T& b ))
            : gt( tgt != ( gtfunc ) undefinedgt<T> ? tgt : cgt )
    {
        if ( gt == ( gtfunc ) undefinedgt<T> )
            throw TemplateFunctionError( "LinkedPriorityQueue::initializer_list constructor: neither specified" );
        if ( tgt != ( gtfunc ) undefinedgt<T> && cgt != ( gtfunc ) undefinedgt<T> && tgt != cgt )
            throw TemplateFunctionError(
                    "LinkedPriorityQueue::initializer_list constructor: both specified and different" );

        for ( const T& pq_elem : il )
        {
            enqueue( pq_elem );
        }
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    template< class Iterable >
    LinkedPriorityQueue<T, tgt>::LinkedPriorityQueue( const Iterable& i, bool (* cgt)( const T& a, const T& b ))
            : gt( tgt != ( gtfunc ) undefinedgt<T> ? tgt : cgt )
    {
        if ( gt == ( gtfunc ) undefinedgt<T> )
            throw TemplateFunctionError( "LinkedPriorityQueue::Iterable constructor: neither specified" );
        if ( tgt != ( gtfunc ) undefinedgt<T> && cgt != ( gtfunc ) undefinedgt<T> && tgt != cgt )
            throw TemplateFunctionError( "LinkedPriorityQueue::Iterable constructor: both specified and different" );

        for ( const T& v : i )
        {
            enqueue( v );
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    bool LinkedPriorityQueue<T, tgt>::empty() const
    {
        return used == 0;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    int LinkedPriorityQueue<T, tgt>::size() const
    {
        return used;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    T& LinkedPriorityQueue<T, tgt>::peek() const
    {
        if ( empty())
        {
            throw EmptyError( "LinkedPriorityQueue::peek" );
        }
        return front->next->value;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    std::string LinkedPriorityQueue<T, tgt>::str() const
    {
        std::ostringstream result;
        result << "LinkedPriorityQueue[";

        if ( used != 0 )
        {
            int count = 0;
            for ( auto* ln = front->next; ln != nullptr; ln = ln->next )
            {
                result << count++ << ":" << ln->value << ( ln->next == nullptr ? "]" : "," );
            }
        }
        else
        {
            result << "]";
        }

        result << "(length=" << used << ",used=" << used << ",mod_count=" << mod_count << ")";
        return result.str();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    int LinkedPriorityQueue<T, tgt>::enqueue( const T& element )
    {
        if ( front->next == nullptr )
        {
            front->next = new LN( element );
        }
        else
        {
            for ( auto* t = front->next; t != nullptr; t = t->next )
            {
                if ( gt( element, t->value ))
                {
                    t->next = new LN( t->value, t->next ); // 'swap' by moving old t value down the linked list..
                    t->value = element; // ..and giving t the new value
                    break;
                }
                else if ( t->next == nullptr )
                {
                    t->next = new LN( element );
                    break;
                }
            }
        }


        ++used;
        ++mod_count;
        return 1;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    T LinkedPriorityQueue<T, tgt>::dequeue()
    {
        if ( empty())
        {
            throw EmptyError( "LinkedPriorityQueue::peek" );
        }

        T dequeuedValue = front->next->value;

        auto* newFrontNextNode = front->next->next;
        delete front->next;
        front->next = newFrontNextNode;

        --used;
        ++mod_count;
        return dequeuedValue;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    void LinkedPriorityQueue<T, tgt>::clear()
    {
        delete_list( front->next );
        used = 0;
        ++mod_count;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    template< class Iterable >
    int LinkedPriorityQueue<T, tgt>::enqueue_all( const Iterable& i )
    {
        int count = 0;
        for ( const T& v : i )
        {
            count += enqueue( v );
        }

        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>& LinkedPriorityQueue<T, tgt>::operator=( const LinkedPriorityQueue<T, tgt>& rhs )
    {
        if ( this == &rhs )
        {
            return *this;
        }

//        delete_list( front->next );
//        front = new LN(); // rm?
//
//        for ( auto* i = front, * j = rhs.front->next; j != nullptr; i = i->next, j = j->next )
//        {
//            i->next = new LN( j->value );
//        }

        if ( used > rhs.used )
        {
            if ( rhs.empty())
            {
                this->clear();
                return *this;
            }

            auto* lhsLn = this->front->next;
            for ( auto* rhsLn = rhs.front->next;; )
            {
                lhsLn->value = rhsLn->value;
                rhsLn = rhsLn->next;
                if ( rhsLn != nullptr )
                {
                    lhsLn = lhsLn->next;
                }
                else
                {
                    break;
                }
            }

            delete_list( lhsLn->next );
        }
        else if ( used < rhs.used )
        {
            auto rhsIter = rhs.begin();

            LN* ln = this->front;
            if ( ln->next != nullptr )
            {
                ln = ln->next;
                for ( ;; ln = ln->next )
                {
                    ln->value = *rhsIter++;

                    if ( ln->next == nullptr )
                    {
                        break;
                    }
                }
            }

            do
            {
                ln->next = new LN( *rhsIter++ );
                ln = ln->next;
            } while ( rhsIter != rhs.end());
        }
        else
        {
            auto rhsIter = rhs.begin();
            for ( auto* t = front->next; rhsIter != rhs.end(); ++rhsIter, t = t->next )
            {
                t->value = *rhsIter;
            }
        }

        gt = rhs.gt;
        used = rhs.used;
        ++mod_count;
        return *this;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    bool LinkedPriorityQueue<T, tgt>::operator==( const LinkedPriorityQueue<T, tgt>& rhs ) const
    {
        if ( this == &rhs )
        {
            return true;
        }
        if ( gt != rhs.gt )
        {
            return false;
        }
        if ( used != rhs.size())
        {
            return false;
        }

        auto j = rhs.begin();
        for ( auto* i = front->next; i != nullptr; i = i->next, ++j )
        {
            if ( i->value != *j )
            {
                return false;
            }
        }

        return true;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    bool LinkedPriorityQueue<T, tgt>::operator!=( const LinkedPriorityQueue<T, tgt>& rhs ) const
    {
        return !( *this == rhs );
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    std::ostream& operator<<( std::ostream& outs, const LinkedPriorityQueue<T, tgt>& pq )
    {
        outs << "priority_queue[";
        ics::ArrayStack<std::string> reverseValues;
        if ( !pq.empty())
        {
            reverseValues.push( pq.front->next->value );
            for ( auto* ln = pq.front->next->next; ln != nullptr; ln = ln->next )
            {
                reverseValues.push( ln->value );
            }

            outs << reverseValues.pop();
            for ( auto i = 1; i < pq.used; ++i )
            {
                outs << "," << reverseValues.pop();
            }
        }

        outs << "]:highest";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    auto LinkedPriorityQueue<T, tgt>::begin() const -> LinkedPriorityQueue<T, tgt>::Iterator
    {
        return Iterator( const_cast<LinkedPriorityQueue <T, tgt>*>(this), front );
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    auto LinkedPriorityQueue<T, tgt>::end() const -> LinkedPriorityQueue<T, tgt>::Iterator
    {
        return Iterator( const_cast<LinkedPriorityQueue <T, tgt>*>(this), nullptr );
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    void LinkedPriorityQueue<T, tgt>::delete_list( LN*& front )
    {
        if ( front != nullptr )
        {
            delete_list( front->next );
            delete front;
            front = nullptr;
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::Iterator::Iterator( LinkedPriorityQueue<T, tgt>* iterate_over, LN* initial )
            : ref_pq( iterate_over ), prev( initial ), expected_mod_count( ref_pq->mod_count )
    {
        if ( initial != nullptr )
        {
            current = initial->next;
        }
        else
        {
            current = nullptr;
        }
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    LinkedPriorityQueue<T, tgt>::Iterator::~Iterator()
    {
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    T LinkedPriorityQueue<T, tgt>::Iterator::erase()
    {
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::erase" );
        if ( !can_erase )
            throw CannotEraseError( "LinkedPriorityQueue::Iterator::erase Iterator cursor already erased" );

        can_erase = false;

        T to_return = current->value;
        prev->next = current->next;
        delete current;
        current = prev->next;

        --ref_pq->used;
        ++ref_pq->mod_count;
        expected_mod_count = ref_pq->mod_count;
        return to_return;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    std::string LinkedPriorityQueue<T, tgt>::Iterator::str() const
    {
        std::ostringstream answer;
        answer << ref_pq->str() << "/current=" << ( current != nullptr ? current->value : "" ) << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
        return answer.str();
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    auto LinkedPriorityQueue<T, tgt>::Iterator::operator++() -> LinkedPriorityQueue<T, tgt>::Iterator&
    {
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator ++" );

        if ( current == nullptr ) // cannot advance past nullptr
            return *this;

        if ( can_erase )
        {
            prev = current;
            current = current->next;
        }
        else
        {
            can_erase = true;
        }

        return *this;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    auto LinkedPriorityQueue<T, tgt>::Iterator::operator++( int ) -> LinkedPriorityQueue<T, tgt>::Iterator
    {
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator ++(int)" );

        if ( current == nullptr ) // cannot advance past nullptr
            return *this;

        Iterator to_return( *this );
        if ( can_erase )
        {
            prev = current;
            current = current->next;
        }
        else
        {
            can_erase = true;
        }

        return to_return;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    bool LinkedPriorityQueue<T, tgt>::Iterator::operator==( const LinkedPriorityQueue<T, tgt>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedPriorityQueue::Iterator::operator ==" );
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator ==" );
        if ( ref_pq != rhsASI->ref_pq )
            throw ComparingDifferentIteratorsError( "LinkedPriorityQueue::Iterator::operator ==" );

        return current == rhsASI->current;
    }


    template< class T, bool (* tgt)( const T& a, const T& b ) >
    bool LinkedPriorityQueue<T, tgt>::Iterator::operator!=( const LinkedPriorityQueue<T, tgt>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedPriorityQueue::Iterator::operator !=" );
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator !=" );
        if ( ref_pq != rhsASI->ref_pq )
            throw ComparingDifferentIteratorsError( "LinkedPriorityQueue::Iterator::operator !=" );

        return current != rhsASI->current;
    }

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    T& LinkedPriorityQueue<T, tgt>::Iterator::operator*() const
    {
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator *" );
        if ( !can_erase )
        {
            std::ostringstream where;
            if ( current == nullptr )
            {
                where << "(nullptr)";
            }
            else
            {
                where << current->value;
            }
            where << " when size = " << ref_pq->size();
            throw IteratorPositionIllegal( "LinkedPriorityQueue::Iterator::operator * Iterator illegal: " + where.str());
        }

        return current->value;
    }

    template< class T, bool (* tgt)( const T& a, const T& b ) >
    T* LinkedPriorityQueue<T, tgt>::Iterator::operator->() const
    {
        if ( expected_mod_count != ref_pq->mod_count )
            throw ConcurrentModificationError( "LinkedPriorityQueue::Iterator::operator *" );
        if ( !can_erase )
        {
            std::ostringstream where;
            if ( current == nullptr )
            {
                where << "(nullptr)";
            }
            else
            {
                where << current->value;
            }
            where << " when size = " << ref_pq->size();
            throw IteratorPositionIllegal( "LinkedPriorityQueue::Iterator::operator * Iterator illegal: " + where.str());
        }

        return &current->value;
    }


}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
