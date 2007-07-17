/*
    Конструкторы / деструктор
*/

template <class T>
inline weak_ptr<T>::weak_ptr ()
  : ptr (0), counter (0)
  { }

template <class T>
inline weak_ptr<T>::weak_ptr (const weak_ptr& p)
  : ptr (p.ptr), counter (p.counter)
{
  if (counter)
    counter->weak_add_ref ();
}

template <class T> template <class T1>
weak_ptr<T>::weak_ptr (const weak_ptr<T1>& p)
  : counter (p.counter)
{
  if (!counter)
  {
    ptr = 0;
    return;
  }

  try
  {
    counter->weak_add_ref ();
      
    ptr = p.lock ().get ();
  }
  catch (...)
  {
    counter->weak_release ();
    throw;
  }
}

template <class T> template <class T1>
inline weak_ptr<T>::weak_ptr (const shared_ptr<T1>& p)
  : ptr (p.ptr), counter (p.counter)
{ 
  if (counter)
    counter->weak_add_ref ();
}

template <class T> template <class T1>
inline weak_ptr<T>::weak_ptr (T1* in_ptr, shared_counter* in_counter)
  : ptr (in_ptr), counter (in_counter)
{
  if (counter)
    counter->weak_add_ref ();
}

template <class T>
inline weak_ptr<T>::~weak_ptr ()
{
  if (counter)
    counter->weak_release ();
}

/*
    Операторы присваивания
*/

template <class T>
inline weak_ptr<T>& weak_ptr<T>::operator = (const weak_ptr& p)
{
  weak_ptr (p).swap (*this);
  
  return *this;
}

template <class T> template <class T1>
inline weak_ptr<T>& weak_ptr<T>::operator = (const weak_ptr<T1>& p)
{
  weak_ptr (p).swap (*this);
  
  return *this;
}

template <class T> template <class T1>
inline weak_ptr<T>& weak_ptr<T>::operator = (const shared_ptr<T1>& p)
{
  weak_ptr (p).swap (*this);

  return *this;
}

template <class T> template <class T1>
inline void weak_ptr<T>::assign (T1* in_ptr, shared_counter* in_counter)
{
  weak_ptr (in_ptr, in_counter).swap (*this);
}

/*
    Сброс
*/

template <class T>
inline void weak_ptr<T>::reset ()
{
  weak_ptr ().swap (*this);
}

/*
    Информация о состоянии указателя    
*/

template <class T>
inline long weak_ptr<T>::use_count () const
{
  return counter ? counter->use_count () : 0;
}

/*
    Получение shared_ptr
*/

template <class T>
shared_ptr<T> weak_ptr<T>::lock () const
{
  if (expired ())
    return shared_ptr<T> ();

  try
  {
    return shared_ptr<T> (*this);
  }
  catch (const bad_weak_ptr&)
  {
    return shared_ptr<T> ();
  }
}

/*
    Обмен
*/

template <class T>
inline void weak_ptr<T>::swap (weak_ptr& p)
{
  T* ptr_tmp = ptr;
  ptr        = p.ptr;
  p.ptr      = ptr_tmp;

  shared_counter* counter_tmp = counter;
  counter                     = p.counter;
  p.counter                   = counter_tmp;
}

template <class T>
inline void swap (weak_ptr<T>& p1, weak_ptr<T>& p2)
{
  p1.swap (p2);
}

/*
    Сравнение указателей
*/

template <class T> template <class T1>
inline bool weak_ptr<T>::operator == (const weak_ptr<T1>& p) const
{
  return ptr == p.ptr;
}

template <class T> template <class T1>
inline bool weak_ptr<T>::operator != (const weak_ptr<T1>& p) const
{
  return !(*this == p);
}

template <class T> template <class T1>
inline bool weak_ptr<T>::operator < (const weak_ptr<T1>& p) const
{
  return counter < p.counter;
}

template <class T> template <class T1>
inline bool weak_ptr<T>::operator > (const weak_ptr<T1>& p) const
{
  return p < *this;
}

template <class T> template <class T1>
inline bool weak_ptr<T>::operator <= (const weak_ptr<T1>& p) const
{
  return (!p < *this);
}

template <class T> template <class T1>
inline bool weak_ptr<T>::operator >= (const weak_ptr<T1>& p) const
{
  return (!*this < p);
}
