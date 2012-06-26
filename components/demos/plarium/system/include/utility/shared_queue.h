#ifndef UTILITY_SHARED_QUEUE_HEADER
#define UTILITY_SHARED_QUEUE_HEADER

#include <cstddef>
#include <sgi_stl/deque>
#include <sgi_stl/memory>

#include <system/condition.h>
#include <system/mutex.h>

namespace plarium
{

namespace utility
{

/// Thread-safe fixed-size multiple threads shared queue.
template <typename T> class SharedQueue
{
  public:
    /// Constructor / destructor
    explicit SharedQueue (size_t size);
            ~SharedQueue ();

    /// Gets the number of elements contained in the Queue.
    bool   Empty () const;
    size_t Size  () const;

    /// Puts the item into the queue.
    bool Enqueue (sgi_stl::auto_ptr<T>& item);

    /// Removes and returns the item at the beginning of the Queue.
    void Dequeue (size_t milliseconds_timeout, sgi_stl::auto_ptr<T>& result);

    /// Clear queue
    void Clear ();

    /// Force wakes up dequeue operation
    void WakeUp ();

  private:
    SharedQueue (const SharedQueue&);             //no impl
    SharedQueue& operator = (const SharedQueue&); //no impl

  private:
    typedef sgi_stl::deque<T*> ItemCollection;

    ItemCollection             items;
    plarium::system::Mutex     mutex;
    plarium::system::Condition dequeue_condition;
    size_t                     queue_size;
};

#include <utility/detail/shared_queue.inl>

}

}

#endif
