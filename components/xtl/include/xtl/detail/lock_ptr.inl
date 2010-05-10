/*
    Конструкторы
*/

template <class T, class Ptr>
inline lock_ptr<T, Ptr>::lock_ptr ()
  : ptr ()
{
}

template <class T, class Ptr>
inline lock_ptr<T, Ptr>::lock_ptr (const pointer& in_ptr)
  : ptr (in_ptr)
{
  lock_object ();
}

template <class T, class Ptr>
inline lock_ptr<T, Ptr>::lock_ptr (const lock_ptr<T, Ptr>& p)
  : ptr (p.ptr)
{
  lock_object ();
}

template <class T, class Ptr> template <class T1>
inline lock_ptr<T, Ptr>::lock_ptr (T1* in_ptr)
  : ptr (static_cast<pointer> (in_ptr))
{
  lock_object ();
}

template <class T, class Ptr> template <class T1, class Ptr1>
inline lock_ptr<T, Ptr>::lock_ptr (const lock_ptr<T1, Ptr1>& p)
  : ptr (static_cast<pointer> (p.get ()))
{
  lock_object ();
}

template <class T, class Ptr>
inline lock_ptr<T, Ptr>::~lock_ptr ()
{
  unlock_object ();
}

/*
    Присваивание
*/

template <class T, class Ptr>
inline lock_ptr<T, Ptr>& lock_ptr<T, Ptr>::operator = (const pointer& in_ptr)
{
  reset (in_ptr);
  return *this;
}

template <class T, class Ptr>
inline lock_ptr<T, Ptr>& lock_ptr<T, Ptr>::operator = (const lock_ptr& p)
{
  reset (p.get ());
  return *this;
}

template <class T, class Ptr> template <class T1>
inline lock_ptr<T, Ptr>& lock_ptr<T, Ptr>::operator = (T1* in_ptr)
{
  reset (in_ptr);
  return *this;
}

template <class T, class Ptr> template <class T1, class Ptr1>
inline lock_ptr<T, Ptr>& lock_ptr<T, Ptr>::operator = (const lock_ptr<T1, Ptr1>& p)
{
  reset (p.get ());
  return *this;
}

template <class T, class Ptr>
inline void lock_ptr<T, Ptr>::reset ()
{
  lock_ptr ().swap (*this);
}

template <class T, class Ptr>
inline void lock_ptr<T, Ptr>::reset (const pointer& in_ptr)
{
  lock_ptr (in_ptr).swap (*this);
}

template <class T, class Ptr> template <class T1>
inline void lock_ptr<T, Ptr>::reset (T1* in_ptr)
{
  lock_ptr (in_ptr).swap (*this);
}

/*
    Запирание
*/

template <class T, class Ptr>
inline void lock_ptr<T, Ptr>::lock_object ()
{
  if (ptr)
    lock (*ptr);
}

template <class T, class Ptr>
inline void lock_ptr<T, Ptr>::unlock_object ()
{
  if (ptr)
    unlock (*ptr);
}

/*
    Операции доступа
*/

template <class T, class Ptr>
inline typename lock_ptr<T, Ptr>::pointer lock_ptr<T, Ptr>::get () const
{
  return ptr;
}

template <class T, class Ptr>
inline T& lock_ptr<T, Ptr>::operator * () const
{
  return *ptr;
}

template <class T, class Ptr>
inline typename lock_ptr<T, Ptr>::pointer lock_ptr<T, Ptr>::operator -> () const
{
  return get ();
}

template <class T, class Ptr>
inline T* get_pointer (lock_ptr<T, Ptr>& p)
{
  return &*p.get ();
}

/*
    Информация о состоянии указателя
*/

template <class T, class Ptr>
inline bool lock_ptr<T, Ptr>::operator ! () const
{
  return ptr.get () == 0;
}

/*
    Сравнение
*/

template <class T, class Ptr>
inline bool lock_ptr<T, Ptr>::operator == (const pointer& p) const
{
  return ptr == p;
}

template <class T, class Ptr> template <class T1, class Ptr1>
inline bool lock_ptr<T, Ptr>::operator == (const lock_ptr<T1, Ptr1>& p) const
{
  return ptr == p.ptr;
}

template <class T, class Ptr> template <class T1>
inline bool lock_ptr<T, Ptr>::operator == (const T1* in_ptr) const
{
  return ptr == in_ptr;
}

template <class T, class Ptr>
inline bool lock_ptr<T, Ptr>::operator != (const pointer& p) const
{
  return ptr != p;
}

template <class T, class Ptr> template <class T1, class Ptr1>
inline bool lock_ptr<T, Ptr>::operator != (const lock_ptr<T1, Ptr1>& p) const
{
  return ptr != p.ptr;
}

template <class T, class Ptr> template <class T1>
inline bool lock_ptr<T, Ptr>::operator != (const T1* in_ptr) const
{
  return ptr != in_ptr;
}

template <class T, class Ptr>
inline bool operator == (const typename lock_ptr<T, Ptr>::pointer& p1, const lock_ptr<T, Ptr>& p2)
{
  return p2 == p1;
}

template <class T, class Ptr>
bool operator != (const typename lock_ptr<T, Ptr>::pointer& p1, const lock_ptr<T, Ptr>& p2)
{
  return p2 != p1;
}

template <class T1, class T2, class Ptr2>
inline bool operator == (const T1* p1, const lock_ptr<T2, Ptr2>& p2)
{
  return p1 == p2.get ();
}

template <class T1, class T2, class Ptr2>
inline bool operator != (const T1* p1, const lock_ptr<T2, Ptr2>& p2)
{
  return p1 != p2.get ();
}

/*
    Обмен
*/

template <class T, class Ptr>
inline void lock_ptr<T, Ptr>::swap (lock_ptr<T, Ptr>& p)
{
  Ptr tmp = p.ptr;
  p.ptr   = ptr;
  ptr     = tmp;
}

template <class T, class Ptr>
inline void swap (lock_ptr<T, Ptr>& p1, lock_ptr<T, Ptr>& p2)
{
  p1.swap (p2);
}
