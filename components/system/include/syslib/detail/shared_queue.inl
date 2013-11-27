/*
    ќписание реализации очереди команд
*/

template <class T, class Allocator> struct SharedQueue<T, Allocator>::Impl
{
  typedef stl::deque<T, Allocator>      Queue;
  typedef xtl::lock_ptr<xtl::spin_lock> Guard;

  struct Unguard
  {
    Unguard (xtl::spin_lock& in_lock) : lock (in_lock) { lock.unlock (); }

    ~Unguard () { lock.lock (); }

    xtl::spin_lock& lock;
  };

  Queue             queue;                         //очередь буферов
  const size_t      max_queue_size;                //максимальный размер очереди
  const size_t      spin_lock_timeout;             //максимальное врем€ ожидани€ в spin-wait
  xtl::spin_lock    lock;                          //примитив синхронизации дл€ быстрого захвата
  syslib::Semaphore full_semaphore;                //событие дл€ реализации системного ожидани€
  syslib::Semaphore empty_semaphore;               //событие дл€ реализации системного ожидани€
  volatile size_t   full_condition_wait_counter;   //количество нитей, ожидающих на событии переполнени€ очереди
  volatile size_t   empty_condition_wait_counter;  //количество нитей, ожидающих на событии пустоты очереди

///  онструктор
  Impl (size_t in_max_queue_size, size_t in_spin_lock_timeout)
    : max_queue_size (in_max_queue_size)
    , spin_lock_timeout (in_spin_lock_timeout)
    , full_semaphore (in_max_queue_size)
    , full_condition_wait_counter ()
    , empty_condition_wait_counter ()
  {
  }

/// ќжидание готовности очереди
  bool WaitUnderflow (size_t timeout)
  {
    size_t start_time = common::milliseconds ();

    for (;;)
    {
      if (!queue.empty ())
        return true;
      
      if (!WaitChanges (start_time, timeout, empty_semaphore, empty_condition_wait_counter))
        return false;
    }
  }

  bool WaitOverflow (size_t timeout)
  {
    size_t start_time = common::milliseconds ();

    for (;;)
    {
      if (queue.size () < max_queue_size)
        return true;
      
      if (!WaitChanges (start_time, timeout, full_semaphore, full_condition_wait_counter))
        return false;
    }
  }

/// ќжидание изменений
  bool WaitChanges (size_t start_time, size_t timeout, syslib::Semaphore& semaphore, volatile size_t& wait_counter)
  {
    const size_t initial_queue_size = queue.size (), loop_timeout = timeout < spin_lock_timeout ? timeout : spin_lock_timeout;

    for (;;)
    {
      if (initial_queue_size != queue.size ())
        return true;

      const size_t cur_time = common::milliseconds (), delay = cur_time - start_time;

      if (delay >= loop_timeout && delay < timeout)
        wait_counter++; //this should be done inside lock

        //system wait

      try
      {
        Unguard unguard (lock); //nothrow

        if (delay < loop_timeout)
          continue;

        if (delay >= timeout)
          return false;  //timeout

        semaphore.TryWait (timeout - delay);
      }
      catch (...)
      {
        wait_counter--; //restore wait counter (under lock)
        throw;
      }
      
        //restore wait counter (under lock)

      wait_counter--;
    }
  }
};

/*
     онструктор / деструктор
*/

template <class T, class Allocator>
inline SharedQueue<T, Allocator>::SharedQueue (size_t max_queue_size, size_t spin_lock_timeout_milliseconds)
  : impl (new Impl (max_queue_size, spin_lock_timeout_milliseconds))
{
}

template <class T, class Allocator>
inline SharedQueue<T, Allocator>::~SharedQueue ()
{
  impl->lock.lock ();
}

/*
    –азмер очереди / проверка на пустоту
*/

template <class T, class Allocator>
inline size_t SharedQueue<T, Allocator>::Size () const
{
  Impl::Guard guard (&impl->lock);

  return impl->queue.size ();
}

template <class T, class Allocator>
inline size_t SharedQueue<T, Allocator>::MaxSize () const
{
  return impl->max_queue_size;
}

template <class T, class Allocator>
inline bool SharedQueue<T, Allocator>::IsEmpty () const
{
  Impl::Guard guard (&impl->lock);

  return impl->queue.empty ();
}

/*
    ѕомещение элемента в очередь / извлечение из очереди
*/

template <class T, class Allocator>
inline void SharedQueue<T, Allocator>::Push (const T& value)
{
  while (!TryPush (value, size_t (-1)));
}

template <class T, class Allocator>
inline void SharedQueue<T, Allocator>::Pop (T& value)
{
  while (!TryPop (value, size_t (-1)));
}

namespace detail
{

template <class T> struct ResultWrapper
{
  T value;

  ResultWrapper () {}
};

}

template <class T, class Allocator>
inline T SharedQueue<T, Allocator>::Pop ()
{
  detail::ResultWrapper<T> result;

  Pop (result.value);

  return result.value;
}

template <class T, class Allocator>
inline bool SharedQueue<T, Allocator>::TryPush (const T& value, size_t timeout)
{
  Impl::Guard guard (&impl->lock);

  if (impl->WaitOverflow (timeout))
  {
    impl->queue.push_back (value);

    if (impl->empty_condition_wait_counter)
      impl->empty_semaphore.Post ();

    return true;
  }

  return false;
}

template <class T, class Allocator>
inline bool SharedQueue<T, Allocator>::TryPop (T& value, size_t timeout)
{
  Impl::Guard guard (&impl->lock);

  if (impl->WaitUnderflow (timeout))
  {
    value = impl->queue.front ();

    impl->queue.pop_front ();

    if (impl->full_condition_wait_counter)
      impl->full_semaphore.Post ();

    return true;
  }

  return false;
}
