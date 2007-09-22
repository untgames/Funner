/*
    intrusive_ptr
*/

/*
    Конструкторы
*/

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>::intrusive_ptr ()
  : ptr (0)
  { }

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>::intrusive_ptr (element_type* p, bool addref)
  : ptr (p ? addref ? strategy_type::clone (p) : p : 0)
  { }

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>::intrusive_ptr (const intrusive_ptr& p)
  : ptr (p.get () ? strategy_type::clone (p.get ()) : 0)
  { }

template <class T, template <class> class Strategy> template <class T1>
inline intrusive_ptr<T, Strategy>::intrusive_ptr (const intrusive_ptr<T1, Strategy>& p)
  : ptr (p.get () ? strategy_type::clone (p.get ()) : 0)
  { }

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>::~intrusive_ptr ()
{
  if (ptr)
    strategy_type::release (ptr);
}

/*
    Присваивание
*/

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>& intrusive_ptr<T, Strategy>::operator = (element_type* p)
{
  if (ptr != p)
    intrusive_ptr (p).swap (*this);

  return *this;
}

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>& intrusive_ptr<T, Strategy>::operator = (const intrusive_ptr& p)
{
  if (ptr != p.get ())
    intrusive_ptr (p).swap (*this);

  return *this;
}

template <class T, template <class> class Strategy> template <class T1>
inline intrusive_ptr<T, Strategy>& intrusive_ptr<T, Strategy>::operator = (const intrusive_ptr<T1, Strategy>& p)
{
  if (p.get () != ptr)
    intrusive_ptr (p).swap (*this);

  return *this;
}

/*
    Получение указателя
*/

template <class T, template <class> class Strategy>
inline T* intrusive_ptr<T, Strategy>::get () const
{
  return ptr;
}

template <class T, template <class> class Strategy>
inline T* intrusive_ptr<T, Strategy>::operator -> () const
{
  return ptr;
}

template <class T, template <class> class Strategy>
inline T& intrusive_ptr<T, Strategy>::operator * () const
{
  return *ptr;
}

/*
    Проверка указателя
*/

template <class T, template <class> class Strategy>
inline intrusive_ptr<T, Strategy>::operator unspecified_bool_type () const
{
  return ptr ? &intrusive_ptr::get : 0;
}

template <class T, template <class> class Strategy>
inline bool intrusive_ptr<T, Strategy>::operator ! () const
{
  return ptr == 0;
}

/*
    Сравнение указателей
*/

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator == (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr == p.get ();
}

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator != (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr != p.get ();
}

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator < (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr < p.get ();
}

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator > (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr > p.get ();
}

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator <= (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr <= p.get ();
}

template <class T,  template <class> class Strategy> template <class T1,  template <class> class Strategy1>
inline bool intrusive_ptr<T,  Strategy>::operator >= (const intrusive_ptr<T1,  Strategy1>& p) const
{
  return ptr >= p.get ();
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator == (const T1* p) const
{
  return ptr == p;
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator != (const T1* p) const
{
  return ptr != p;
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator < (const T1* p) const
{
  return ptr < p;
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator > (const T1* p) const
{
  return ptr > p;
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator <= (const T1* p) const
{
  return ptr <= p;
}

template <class T,  template <class> class Strategy> template <class T1>
inline bool intrusive_ptr<T,  Strategy>::operator >= (const T1* p) const
{
  return ptr >= p;
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator == (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 == p2.get ();
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator != (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 != p2.get ();
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator < (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 < p2.get ();
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator > (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 > p2.get ();
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator <= (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 <= p2.get ();
}

template <class T1, class T2, template <class> class Strategy>
inline bool operator >= (const T1* p1, const intrusive_ptr<T2, Strategy>& p2)
{
  return p1 >= p2.get ();
}

/*
    Обмен значений указателей
*/

template <class T, template <class> class Strategy>
inline void intrusive_ptr<T, Strategy>::swap (intrusive_ptr& p)
{
  T* tmp = ptr;
  ptr    = p.ptr;
  p.ptr  = tmp;
}

template <class T, template <class> class Strategy>
inline void swap (intrusive_ptr<T, Strategy>& p1, intrusive_ptr<T, Strategy>& p2)
{
  p1.swap (p2);
}

/*
    Приведение типов
*/

template <class T1, class T2, template <class> class Strategy>
inline intrusive_ptr<T1, Strategy> static_pointer_cast (const intrusive_ptr<T2, Strategy>& p)
{
  return static_cast<T1*> (p.get ());
}

template <class T1, class T2, template <class> class Strategy>
inline intrusive_ptr<T1, Strategy> const_pointer_cast (const intrusive_ptr<T2, Strategy>& p)
{
  return const_cast<T1*> (p.get ());
}

template <class T1, class T2, template <class> class Strategy>
inline intrusive_ptr<T1, Strategy> dynamic_pointer_cast (const intrusive_ptr<T2, Strategy>& p)
{
  return dynamic_cast<T1*> (p.get ());
}

/*
    Получение plain-указателя
*/

template <class T, template <class> class Strategy>
inline T* get_pointer (const intrusive_ptr<T, Strategy>& p)
{
  return p.get ();
}

/*
    com_ptr
*/

template <class T>
inline com_ptr<T>::com_ptr ()
  : base ()
  { }

template <class T>
inline com_ptr<T>::com_ptr (element_type* p, bool addref)
  : base (p, addref)
  { }

template <class T>
inline com_ptr<T>::com_ptr (const com_ptr& p)
  : base (p)
  { }

template <class T> template <class T1>
inline com_ptr<T>::com_ptr (const com_ptr<T1>& p)
  : base (p)
  { }

/*
    Обмен значений указателей
*/

template <class T>
inline void swap (com_ptr<T>& p1, com_ptr<T>& p2)
{
  p1.swap (p2);  
}  

/*
    Приведение типов
*/

template <class T1, class T2>
inline com_ptr<T1> static_pointer_cast (const com_ptr<T2>& p)
{
  return static_cast<T1*> (p.get ());
}

template <class T1, class T2>
inline com_ptr<T1> const_pointer_cast (const com_ptr<T2>& p)
{
  return const_cast<T1*> (p.get ());
}

template <class T1, class T2>
inline com_ptr<T1> dynamic_pointer_cast (const com_ptr<T2>& p)
{
  return dynamic_cast<T1*> (p.get ());
}
