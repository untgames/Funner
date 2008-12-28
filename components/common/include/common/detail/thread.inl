#if 0

/*
    ThreadLockScope
*/

template <class T>
inline ThreadLockScope<T>::ThreadLockScope (T& in_object)
  : object (&in_object)
{
  thread_lock (*object);
}

template <class T>
inline ThreadLockScope<T>::~ThreadLockScope ()
{
  thread_unlock (*object);
}

template <class T>
inline T* ThreadLockScope<T>::Get () const
{
  return object;
}

template <class T>
inline T* ThreadLockScope<T>::operator -> () const
{
  return object;
}

/*
    Перегрузки запираний различных объектов синхронизации
*/

inline void thread_lock (ThreadLockable& lockable)
{
  lockable.Lock ();
}

inline void thread_unlock (ThreadLockable& lockable)
{
  lockable.Unlock ();
}


#endif
