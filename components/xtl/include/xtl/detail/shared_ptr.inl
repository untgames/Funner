/*
    Конструкторы / деструктор
*/

template <class T>
inline shared_ptr<T>::shared_ptr ()
  : ptr (0), counter (0)
  { }

template <class T> template <class T1>
shared_ptr<T>::shared_ptr (T1* in_ptr)
  : ptr (in_ptr)
{
  try
  {
    counter = new detail::std_shared_counter<T1> (in_ptr);
  }
  catch(...)
  {
    delete in_ptr;
    throw;
  }

  sp_enable_shared_from_this (in_ptr, in_ptr);
}

template <class T> template <class T1, class Deleter>
shared_ptr<T>::shared_ptr (T1* in_ptr, Deleter deleter)
  : ptr (in_ptr)
{
  try
  {
    counter = new detail::user_shared_counter<T1*, Deleter> (in_ptr, deleter);
  }
  catch(...)
  {
    deleter (in_ptr);
    throw;
  }

  sp_enable_shared_from_this (in_ptr, in_ptr);
}    

template <class T>
inline shared_ptr<T>::shared_ptr (const shared_ptr& p)
  : ptr (p.ptr), counter (p.counter)
{
  if (counter)
    counter->add_ref_copy ();
}

template <class T> template <class T1>
inline shared_ptr<T>::shared_ptr (const shared_ptr<T1>& p)
  : ptr (p.ptr), counter (p.counter)
{
  if (counter)
    counter->add_ref_copy ();
}

template <class T> template <class T1>
shared_ptr<T>::shared_ptr (const weak_ptr<T1>& p)
  : counter (p.counter), ptr (p.ptr)
{
  if (!counter || !counter->add_ref_lock ())
    throw bad_weak_ptr ();
}

template <class T> template <class T1>
inline shared_ptr<T>::shared_ptr (stl::auto_ptr<T1>& p)
  : ptr (p.get ())
{
  T1* tmp = p.get ();
  
  counter = new detail::std_shared_counter<T1> (p.get ());

  p.release ();
        
  sp_enable_shared_from_this (tmp, tmp);
}

template <class T> template <class T1>
inline shared_ptr<T>::shared_ptr (const shared_ptr<T1>& p, detail::static_cast_tag)
  : ptr (static_cast<T*> (p.ptr)), counter (p.counter)
{
  if (counter)
    counter->add_ref_copy ();
}

template <class T> template <class T1>
inline shared_ptr<T>::shared_ptr (const shared_ptr<T1>& p, detail::dynamic_cast_tag)
  : ptr (dynamic_cast<T*> (p.ptr)), counter (p.counter)
{
  if      (!ptr)    counter = 0;
  else if (counter) counter->add_ref_copy ();
}

template <class T> template <class T1>
inline shared_ptr<T>::shared_ptr (const shared_ptr<T1>& p, detail::const_cast_tag)
  : ptr (const_cast<T*> (p.ptr)), counter (p.counter)
{
  if (counter)
    counter->add_ref_copy ();
}

template <class T>
inline shared_ptr<T>::~shared_ptr ()
{
  if (counter)
    counter->release ();
}

/*
    Функции для инициализации weak_ptr в объектах класса enable_shared_from_this
*/

template <class T> template <class T1, class T2>
inline void shared_ptr<T>::sp_enable_shared_from_this (const enable_shared_from_this<T1>* p1, const T2* p2) const
{
  if (p1)
    p1->weak_this.assign (const_cast<T2*> (p2), counter);
}

template <class T>
inline void shared_ptr<T>::sp_enable_shared_from_this (...) const
{
}

/*
    Операторы присваивания
*/

template <class T>
inline shared_ptr<T>& shared_ptr<T>::operator = (const shared_ptr& p)
{
  shared_ptr (p).swap (*this);

  return *this;
}

template <class T> template <class T1>
inline shared_ptr<T>& shared_ptr<T>::operator = (const shared_ptr<T1>& p)
{
  shared_ptr (p).swap (*this);
  
  return *this;
}

template <class T> template <class T1>
inline shared_ptr<T>& shared_ptr<T>::operator = (stl::auto_ptr<T1>& p)
{
  shared_ptr (p).swap (*this);

  return *this;  
}

/*
    Операции доступа
*/

template <class T>
inline T* shared_ptr<T>::get () const
{
  return ptr;
}

template <class T>
inline typename detail::shared_ptr_traits<T>::reference shared_ptr<T>::operator * () const
{
  return *ptr;
}

template <class T>
inline T* shared_ptr<T>::operator -> () const
{
  return ptr;
}

template <class T>
inline T* get_pointer (T* p)
{
  return p;
}

template <class T>
inline T* get_pointer (const shared_ptr<T>& p)
{
  return p.get ();
}

/*
    Информация о состоянии указателя
*/

template <class T>
inline long shared_ptr<T>::use_count () const
{
  return counter ? counter->use_count () : 0;
}

template <class T>
inline bool shared_ptr<T>::unique () const
{
  return use_count () == 1;
}
    
template <class T>
inline shared_ptr<T>::operator unspecified_bool_type () const
{
  return ptr ? &shared_ptr::get : 0;
}

template <class T>
inline bool shared_ptr<T>::operator ! () const
{
  return ptr == 0;
}

/*
    Установка нового значения
*/

template <class T>
inline void shared_ptr<T>::reset ()
{
  shared_ptr ().swap (*this);
}

template <class T> template <class T1>
inline void shared_ptr<T>::reset (T1* in_ptr)
{
  shared_ptr (in_ptr).swap (*this);
}

template <class T> template <class T1, class Deleter>
inline void shared_ptr<T>::reset (T1* in_ptr, Deleter deleter)
{
  shared_ptr (in_ptr, deleter).swap (*this);
}

/*
    Обмен
*/

template <class T>
inline void shared_ptr<T>::swap (shared_ptr& p)
{
  T* ptr_tmp = ptr;
  ptr        = p.ptr;
  p.ptr      = ptr_tmp;

  shared_counter* counter_tmp = counter;
  counter                     = p.counter;
  p.counter                   = counter_tmp;
}

template <class T>
inline void swap (shared_ptr<T>& p1, shared_ptr<T>& p2)
{
  p1.swap (p2);
}

/*
    Получение удаляющего функтора
*/

template <class T> template <class Deleter>
inline Deleter* shared_ptr<T>::get_deleter () const
{
  return counter ? static_cast<Deleter*> (counter->get_deleter (typeid (Deleter))) : 0;
}

template <class Deleter, class T>
inline Deleter* get_deleter (const shared_ptr<T>& p)
{
  return p.get_deleter<Deleter> ();
}

/*
    Сравнение указателей
*/

template <class T> template <class T1>
inline bool shared_ptr<T>::operator == (const shared_ptr<T1>& p) const
{
  return ptr == p.ptr;
}

template <class T> template <class T1>
inline bool shared_ptr<T>::operator != (const shared_ptr<T1>& p) const
{
  return !(*this == p);
}

template <class T> template <class T1>
inline bool shared_ptr<T>::operator < (const shared_ptr<T1>& p) const
{
  return counter < p.counter;
}

template <class T> template <class T1>
inline bool shared_ptr<T>::operator > (const shared_ptr<T1>& p) const
{
  return p < *this;
}

template <class T> template <class T1>
inline bool shared_ptr<T>::operator <= (const shared_ptr<T1>& p) const
{
  return (!p < *this);
}

template <class T> template <class T1>
inline bool shared_ptr<T>::operator >= (const shared_ptr<T1>& p) const
{
  return (!*this < p);
}

/*
    Приведение указателей
*/

template <class T1, class T2>
inline shared_ptr<T1> static_pointer_cast (const shared_ptr<T2>& p)
{
  return shared_ptr<T1> (p, detail::static_cast_tag ());
}

template <class T1, class T2>
inline shared_ptr<T1> dynamic_pointer_cast (const shared_ptr<T2>& p)
{
  return shared_ptr<T1> (p, detail::dynamic_cast_tag ());
}

template <class T1, class T2>
inline shared_ptr<T1> const_pointer_cast (const shared_ptr<T2>& p)
{
  return shared_ptr<T1> (p, detail::const_cast_tag ());
}

/*
    enable_shared_from_this
*/

template <class T>
inline shared_ptr<T> enable_shared_from_this<T>::shared_from_this ()
{
  return shared_ptr<T> (weak_this);
}

template <class T>
inline shared_ptr<const T> enable_shared_from_this<T>::shared_from_this () const
{
  return shared_ptr<const T> (weak_this);
}
