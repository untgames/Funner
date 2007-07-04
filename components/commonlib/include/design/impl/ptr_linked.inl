/*
    linked_ptr
*/

/*
    Конструкторы / деструктор
*/

template <class T>
inline linked_ptr<T>::linked_ptr ()
{
  prev_ptr = next_ptr = this;
}

template <class T>
inline linked_ptr<T>::linked_ptr (element_type* ptr)
  : base (ptr)
{
  prev_ptr = next_ptr = this;
}

template <class T>
inline linked_ptr<T>::linked_ptr (element_type* ptr,linked_ptr& next)
  : base (ptr)
{
  connect_internal (next);
}

template <class T>
inline linked_ptr<T>::linked_ptr (parameter_type ptr)
  : base (ptr)
{
  prev_ptr = next_ptr = this;
}

template <class T>
inline linked_ptr<T>::linked_ptr (parameter_type ptr,linked_ptr& next)
  : base (ptr)
{
  connect_internal (next);
}
  
template <class T> template <class T1>
inline linked_ptr<T>::linked_ptr (const linked_ptr<T1>& ptr)
  : base (ptr)
{
  prev_ptr = next_ptr = this;
}

template <class T> template <class T1>
inline linked_ptr<T>::linked_ptr (const linked_ptr<T1>& ptr,linked_ptr& next)
  : base (ptr)
{
  connect_internal (next);
}

template <class T> template <class T1>
inline linked_ptr<T>::linked_ptr (linked_ptr<T1>& ptr)
  : base (ptr)
{
  prev_ptr = next_ptr = this;
}

template <class T> template <class T1>
inline linked_ptr<T>::linked_ptr (linked_ptr<T1>& ptr,linked_ptr& next)
  : base (ptr)
{
  connect_internal (next);
}

template <class T>
inline linked_ptr<T>::~linked_ptr ()
{
  disconnect ();
}

/*
    Присваивание
*/

template <class T>
inline linked_ptr<T>& linked_ptr<T>::operator = (element_type* ptr)
{
  base::operator = (ptr);
  
  return *this;
}

template <class T>
inline linked_ptr<T>& linked_ptr<T>::operator = (parameter_type ptr)
{
  base::operator = (ptr);
  
  return *this;
}

template <class T> template <class T1>
inline linked_ptr<T>& linked_ptr<T>::operator = (const linked_ptr<T1>& ptr)
{
  base::operator = (ptr);
  
  return *this;
}

template <class T> template <class T1>
inline linked_ptr<T>& linked_ptr<T>::operator = (linked_ptr<T1>& ptr)
{
  base::operator = (ptr);
  
  return *this;
}

/*
    Управление связями
*/

template <class T>
inline void linked_ptr<T>::connect_internal (linked_ptr& next)
{
  next_ptr      = &next;
  prev_ptr      = next.prev_ptr;
  next.prev_ptr = prev_ptr->next_ptr = this;
}

template <class T>
inline void linked_ptr<T>::update_links ()
{
  prev_ptr->next_ptr = next_ptr->prev_ptr = this;
}

template <class T>
inline void linked_ptr<T>::connect (linked_ptr& next)
{
  disconnect ();
  connect_internal (next);
}

template <class T>
inline void linked_ptr<T>::disconnect ()
{
  if (next_ptr == this)
    return;

  prev_ptr->next_ptr = next_ptr;
  next_ptr->prev_ptr = prev_ptr;
  prev_ptr           = next_ptr = this;
}

template <class T>
inline bool linked_ptr<T>::connected () const
{
  return next_ptr != this;
}

/*
    Операции доступа
*/

template <class T>
inline linked_ptr<T>& linked_ptr<T>::prev ()
{
  return *prev_ptr;
}

template <class T>
inline const linked_ptr<T>& linked_ptr<T>::prev () const
{
  return *prev_ptr;
}

template <class T>
inline linked_ptr<T>& linked_ptr<T>::next ()
{
  return *next_ptr;
}

template <class T>
inline const linked_ptr<T>& linked_ptr<T>::next () const
{
  return *next_ptr;
}

/*
    Получение итератора
*/

template <class T>
inline typename linked_ptr<T>::iterator linked_ptr<T>::get_iterator () const
{
  return iterator (*this);
}

/*
    Обмен
*/

template <class T>
inline void linked_ptr<T>::swap (linked_ptr& ptr)
{
  base::swap (ptr);

  detail::swap_internal (prev_ptr,ptr.prev_ptr);
  detail::swap_internal (next_ptr,ptr.next_ptr);

  update_links ();
  ptr.update_links ();
}

template <class T>
inline void swap (linked_ptr<T>& a,linked_ptr<T>& b)
{
  a.swap (b);
}

/*
    linked_ptr_iterator
*/

/*
    Конструкторы
*/

template <class Ptr>
inline linked_ptr_iterator<Ptr>::linked_ptr_iterator ()
  : current ()
  { }

template <class Ptr>
inline linked_ptr_iterator<Ptr>::linked_ptr_iterator (const linked_ptr_iterator& i)
  : current (i.current)
  { }

template <class Ptr>
inline linked_ptr_iterator<Ptr>::linked_ptr_iterator (const linked_ptr& ptr)
  : current (&ptr)
  { }

/*
    Присваивание
*/

template <class Ptr>  
inline linked_ptr_iterator<Ptr>& linked_ptr_iterator<Ptr>::operator = (const linked_ptr_iterator& i)
{
  current = i.current;
  return *this;
}

template <class Ptr>  
inline linked_ptr_iterator<Ptr>& linked_ptr_iterator<Ptr>::operator = (const linked_ptr& ptr)
{
  current = &ptr;
  return *this;
}

/*
    Селекторы
*/

template <class Ptr>
inline typename linked_ptr_iterator<Ptr>::reference linked_ptr_iterator<Ptr>::operator * () const
{
  return **current;
}

template <class Ptr>
inline typename linked_ptr_iterator<Ptr>::pointer linked_ptr_iterator<Ptr>::operator -> () const
{
  return &**current;
}

template <class Ptr>
inline typename linked_ptr_iterator<Ptr>::linked_ptr& linked_ptr_iterator<Ptr>::get_link () const
{
  return *current;
}

/*
    Итерация
*/

template <class Ptr>
inline linked_ptr_iterator<Ptr>& linked_ptr_iterator<Ptr>::operator ++ ()
{
  current = &current->next ();
  return *this;
}

template <class Ptr>
inline linked_ptr_iterator<Ptr>& linked_ptr_iterator<Ptr>::operator -- ()
{
  current = &current->prev ();
  return *this;
}

template <class Ptr>
inline const linked_ptr_iterator<Ptr> linked_ptr_iterator<Ptr>::operator ++ (int)
{
  linked_ptr_iterator old = *this;

  ++*this;

  return old;
}

template <class Ptr>
inline const linked_ptr_iterator<Ptr> linked_ptr_iterator<Ptr>::operator -- (int)
{
  linked_ptr_iterator old = *this;

  --*this;

  return old;
}

/*
    Сравнение
*/

template <class Ptr>
inline bool linked_ptr_iterator<Ptr>::operator == (const linked_ptr_iterator& i) const
{
  return current == i.current;
}

template <class Ptr>
inline bool linked_ptr_iterator<Ptr>::operator != (const linked_ptr_iterator& i) const
{
  return !(*this == i);
}
