#ifndef SYSLIB_SHARED_QUEUE_HEADER
#define SYSLIB_SHARED_QUEUE_HEADER

#include <stl/auto_ptr.h>
#include <stl/deque>

#include <xtl/lock_ptr.h>
#include <xtl/spin_lock.h>

#include <common/time.h>

#include <syslib/semaphore.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Потоко безопасная очередь команд с низкой латентностью
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Allocator = typename stl::default_allocator<T>::allocator_type> class SharedQueue
{
  public:
    enum { DEFAULT_SPIN_LOCK_TIMEOUT_MILLISECONDS = 10 };

    typedef T value_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SharedQueue  (size_t max_queue_size, size_t spin_lock_timeout_milliseconds = DEFAULT_SPIN_LOCK_TIMEOUT_MILLISECONDS);
    ~SharedQueue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер очереди / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const;
    size_t MaxSize () const;
    bool   IsEmpty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Помещение элемента в очередь / извлечение из очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Push    (const T& value);
    bool TryPush (const T& value, size_t timeout);
    void Pop     (T& value);
    T    Pop     ();
    bool TryPop  (T& value, size_t timeout);

  private:
    SharedQueue (const SharedQueue&); //no impl
    SharedQueue& operator = (const SharedQueue&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

#include <syslib/detail/shared_queue.inl>

}

#endif
