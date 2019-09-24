#ifndef LINKED_QUEUE_HPP_
#define LINKED_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics
{


    template< class T >
    class LinkedQueue
    {
    public:
        //Destructor/Constructors
        ~LinkedQueue();

        LinkedQueue();

        LinkedQueue( const LinkedQueue<T>& to_copy );

        explicit LinkedQueue( const std::initializer_list<T>& il );

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template< class Iterable >
        explicit LinkedQueue( const Iterable& i );


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
        LinkedQueue<T>& operator=( const LinkedQueue<T>& rhs );

        bool operator==( const LinkedQueue<T>& rhs ) const;

        bool operator!=( const LinkedQueue<T>& rhs ) const;

        template< class T2 >
        friend std::ostream& operator<<( std::ostream& outs, const LinkedQueue<T2>& q );


    private:
        class LN;

    public:
        class Iterator
        {
        public:
            //Private constructor called in begin/end, which are friends of LinkedQueue<T>
            ~Iterator();

            T erase();

            std::string str() const;

            LinkedQueue<T>::Iterator& operator++();

            LinkedQueue<T>::Iterator operator++( int );

            bool operator==( const LinkedQueue<T>::Iterator& rhs ) const;

            bool operator!=( const LinkedQueue<T>::Iterator& rhs ) const;

            T& operator*() const;

            T* operator->() const;

            friend std::ostream& operator<<( std::ostream& outs, const LinkedQueue<T>::Iterator& i )
            {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }

            friend Iterator LinkedQueue<T>::begin() const;

            friend Iterator LinkedQueue<T>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            LN* prev = nullptr;  //if nullptr, current at front of list
            LN* current;         //current == prev->next (if prev != nullptr)
            LinkedQueue<T>* ref_queue;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator( LinkedQueue<T>* iterate_over, LN* initial );
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


        LN* front = nullptr;
        LN* rear = nullptr;
        int used = 0;            //Cache count for nodes in linked list
        int mod_count = 0;            //Alllows sensing for concurrent modification

        //Helper methods
        void delete_list( LN*& front );  //Deallocate all LNs, and set front's argument to nullptr;
    };





////////////////////////////////////////////////////////////////////////////////
//
//LinkedQueue class and related definitions

//Destructor/Constructors

    template< class T >
    LinkedQueue<T>::~LinkedQueue()
    {
        this->clear();
    }


    template< class T >
    LinkedQueue<T>::LinkedQueue()
    {
    }


    template< class T >
    LinkedQueue<T>::LinkedQueue( const LinkedQueue<T>& to_copy ) : used( to_copy.used )
    {
        front = new LN();
        rear = front;
        for ( const T& v : to_copy )
        {
            rear->value = v;
            rear->next = new LN();
            rear = rear->next;
        }
    }


    template< class T >
    LinkedQueue<T>::LinkedQueue( const std::initializer_list<T>& il )
    {
        for ( const T& v : il )
        {
            enqueue( v );
        }
    }


    template< class T >
    template< class Iterable >
    LinkedQueue<T>::LinkedQueue( const Iterable& i )
    {
        for ( const T& v : i )
        {
            enqueue( v );
        }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template< class T >
    bool LinkedQueue<T>::empty() const
    {
        return used == 0;
    }


    template< class T >
    int LinkedQueue<T>::size() const
    {
        return used;
    }


    template< class T >
    T& LinkedQueue<T>::peek() const
    {
        if ( this->empty())
        {
            throw EmptyError( "ArrayQueue::peek" );
        }

        return front->value;
    }


    template< class T >
    std::string LinkedQueue<T>::str() const
    {
        std::ostringstream result;
        result << "LinkedQueue[";

        if ( used != 0 )
        {
            int count = 0;
            for ( auto* ln = front; ln != nullptr; ln = ln->next )
            {
                result << count++ << ":" << ln->value << ( ln->next == nullptr ? "]" : "," );
            }
        }
        else
        {
            result << "]";
        }

        result << "(length=" << used << ",front=" << 0 << ",rear=" << ( used - 1 ) << ",mod_count=" << mod_count << ")";
        return result.str();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template< class T >
    int LinkedQueue<T>::enqueue( const T& element )
    {
        if ( front == nullptr )
        {
            front = new LN( element );
            rear = front;
        }
        else if ( rear->next == nullptr )
        {
            rear->next = new LN( element );
            rear = rear->next;
        }

        ++used;
        ++mod_count;

        return 1;
    }


    template< class T >
    T LinkedQueue<T>::dequeue()
    {
        if ( this->empty())
        {
            throw EmptyError( "LinkedQueue::dequeue" );
        }

        auto val = front->value;

        auto newFront = front->next;
        delete front;
        front = newFront;

        ++mod_count;
        --used;

        return val;
    }


    template< class T >
    void LinkedQueue<T>::clear()
    {
        delete_list( front );
        used = 0;
        ++mod_count;
    }


    template< class T >
    template< class Iterable >
    int LinkedQueue<T>::enqueue_all( const Iterable& i )
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

    template< class T >
    LinkedQueue<T>& LinkedQueue<T>::operator=( const LinkedQueue<T>& rhs )
    {
        if ( this == &rhs )
        {
            return *this;
        }

        if ( used > rhs.used )
        {
            if ( rhs.empty())
            {
                this->clear();
                return *this;
            }

            rear = front; // use rear to traverse list from the front
            for ( auto* rhsLn = rhs.front;; )
            {
                rear->value = rhsLn->value;
                rhsLn = rhsLn->next;
                if ( rhsLn != nullptr )
                {
                    rear = rear->next;
                }
                else
                {
                    break;
                }
            }

            delete_list(rear->next); // set rear->next to nullptr while also deleting unused nodes
        }
        else if ( used < rhs.used )
        {
            if ( front == nullptr )
            {
                front = new LN();
                rear = front;
            }

            auto rhsIter = rhs.begin();

            // Copy the values from rhs until the end of current LHS
            for ( auto* ln = this->front;; ln = ln->next )
            {
                ln->value = *rhsIter++;

                // If end of LHS, stop
                if ( ln->next == nullptr )
                {
                    rear = ln;
                    break;
                }
            }

            // Get rest of values
            do
            {
                rear->next = new LN( *rhsIter++ );
                rear = rear->next;
            } while ( rhsIter != rhs.end());

        }
        else
        {

            // Copy all the values over since size are equal
            auto* ln = this->front;
            for ( const auto& v : rhs )
            {
                ln->value = v;
                ln = ln->next;
            }
            rear = ln;
        }

        used = rhs.used;
        ++mod_count;
        return *this;
    }


    template< class T >
    bool LinkedQueue<T>::operator==( const LinkedQueue<T>& rhs ) const
    {
        if ( this == &rhs )
        {
            return true;
        }

        if ( used != rhs.size())
        {
            return false;
        }

        auto rhs_i = rhs.begin();
        for ( auto* ln = front; ln != nullptr; ln = ln->next, ++rhs_i )
        {
            if ( ln->value != *rhs_i )
            {
                return false;
            }
        }

        return true;
    }


    template< class T >
    bool LinkedQueue<T>::operator!=( const LinkedQueue<T>& rhs ) const
    {
        return !( *this == rhs );
    }


    template< class T >
    std::ostream& operator<<( std::ostream& outs, const LinkedQueue<T>& q )
    {
        outs << "queue[";

        if ( !q.empty())
        {
            outs << q.front->value;
            for ( auto* ln = q.front->next; ln != nullptr; ln = ln->next )
            {
                outs << "," << ln->value;
            }
        }

        outs << "]:rear";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template< class T >
    auto LinkedQueue<T>::begin() const -> LinkedQueue<T>::Iterator
    {
        return Iterator( const_cast<LinkedQueue<T>*>(this), front );
    }

    template< class T >
    auto LinkedQueue<T>::end() const -> LinkedQueue<T>::Iterator
    {
        return Iterator( const_cast<LinkedQueue<T>*>(this), nullptr );
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template< class T >
    void LinkedQueue<T>::delete_list( LN*& front )
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

    template< class T >
    LinkedQueue<T>::Iterator::Iterator( LinkedQueue<T>* iterate_over, LN* initial )
            : ref_queue( iterate_over ), current( initial ), expected_mod_count( ref_queue->mod_count )
    {
    }


    template< class T >
    LinkedQueue<T>::Iterator::~Iterator()
    {
    }


    template< class T >
    T LinkedQueue<T>::Iterator::erase()
    {
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::erase" );
        if ( !can_erase )
            throw CannotEraseError( "LinkedQueue::Iterator::erase Iterator cursor already erased" );

        can_erase = false;

        T to_return = current->value;
        if ( prev == nullptr )
        {
            // front of the list
            auto* next = current->next;
            delete current;
            current = next;
            ref_queue->front = next; // fix up the front of the queue
        }
        else
        {
            prev->next = current->next;
            delete current;
            current = prev->next;

            if ( current == nullptr )
            {
                // fix up the end of the queue if removing at end
                //current = prev;
                ref_queue->rear = prev;
            }
        }

        --ref_queue->used;
        ++ref_queue->mod_count;
        expected_mod_count = ref_queue->mod_count;
        return to_return;
    }


    template< class T >
    std::string LinkedQueue<T>::Iterator::str() const
    {
        std::ostringstream answer;
        answer << ref_queue->str() << "(current=" << ( current != nullptr ? current->value : "" )
               << ",expected_mod_count=" << expected_mod_count
               << ",can_erase=" << can_erase << ")";
        return answer.str();
    }


    template< class T >
    auto LinkedQueue<T>::Iterator::operator++() -> LinkedQueue<T>::Iterator&
    {
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator ++" );

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


    template< class T >
    auto LinkedQueue<T>::Iterator::operator++( int ) -> LinkedQueue<T>::Iterator
    {
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator ++(int)" );

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


    template< class T >
    bool LinkedQueue<T>::Iterator::operator==( const LinkedQueue<T>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedQueue::Iterator::operator ==" );
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator ==" );
        if ( ref_queue != rhsASI->ref_queue )
            throw ComparingDifferentIteratorsError( "LinkedQueue::Iterator::operator ==" );

        return current == rhsASI->current;
    }


    template< class T >
    bool LinkedQueue<T>::Iterator::operator!=( const LinkedQueue<T>::Iterator& rhs ) const
    {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if ( rhsASI == 0 )
            throw IteratorTypeError( "LinkedQueue::Iterator::operator !=" );
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator !=" );
        if ( ref_queue != rhsASI->ref_queue )
            throw ComparingDifferentIteratorsError( "LinkedQueue::Iterator::operator !=" );

        return current != rhsASI->current;
    }


    template< class T >
    T& LinkedQueue<T>::Iterator::operator*() const
    {
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator *" );
        if ( !can_erase )
        {
            std::ostringstream where;
            where << current
                  << " when front = " << ref_queue->front
                  << " and rear = " << ref_queue->rear;
            throw IteratorPositionIllegal( "LinkedQueue::Iterator::operator * Iterator illegal: " + where.str());
        }

        return current->value;
    }


    template< class T >
    T* LinkedQueue<T>::Iterator::operator->() const
    {
        if ( expected_mod_count != ref_queue->mod_count )
            throw ConcurrentModificationError( "LinkedQueue::Iterator::operator ->" );
        if ( !can_erase )
        {
            std::ostringstream where;
            where << current
                  << " when front = " << ref_queue->front
                  << " and rear = " << ref_queue->rear;
            throw IteratorPositionIllegal( "LinkedQueue::Iterator::operator -> Iterator illegal: " + where.str());
        }

        return &current->value;
    }


}

#endif /* LINKED_QUEUE_HPP_ */
