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

/*inline void thread_lock (ThreadLockable& lockable)
{
  lockable.Lock ();
}

inline void thread_unlock (ThreadLockable& lockable)
{
  lockable.Unlock ();
}*/

/*
    ThreadLocalPointer
*/

/*namespace detail
{

template <class T> struct ThreadLocalPointerCleanup
{
  typedef typename ThreadLocalPointer<T>::CleanupHandler CleanupHandler;

  ThreadLocalPointerCleanup (const CleanupHandler& in_handler) : handler (in_handler) {}

  void operator () (void* data) const { handler (static_cast<T*> (data)); }

  CleanupHandler handler;
};

}

template <class T>
inline ThreadLocalPointer<T>::ThreadLocalPointer ()
{
}

template <class T>
inline ThreadLocalPointer<T>::ThreadLocalPointer (const CleanupHandler& cleanup)
  : storage (detail::ThreadLocalPointerCleanup<T> (cleanup))
{
}

template <class T>
inline T* Get () const
{
  return static_cast<T*> (storage.GetValue ());
}

template <class T>
inline T* ThreadLocalPointer<T>::operator -> () const
{
  return Get ();
}

template <class T>
inline T& ThreadLocalPointer<T>::operator * () const
{
  return *Get ();
}

template <class T>
inline void ThreadLocalPointer<T>::Reset (T* new_object)
{
  //???
}

template <class T>
inline T* ThreadLocalPointer<T>::Release ()
{
}
*/
