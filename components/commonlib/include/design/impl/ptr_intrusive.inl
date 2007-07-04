/*
    default_intrusive_strategy
*/

template <class T>
inline T* default_intrusive_strategy<T>::clone (T* p)
{
  intrusive_ptr_addref (p);
  return p;
}

template <class T>
inline void default_intrusive_strategy<T>::release (T* p)
{
  intrusive_ptr_release (p);
}

/*
    intrusive_ptr
*/

/*
    Конструкторы
*/

template <class T,class Strategy>
inline intrusive_ptr<T,Strategy>::intrusive_ptr (element_type* p,bool addref)
  : base (p ? addref ? Strategy::clone (p) : p : 0)
  { }

template <class T,class Strategy>
inline intrusive_ptr<T,Strategy>::intrusive_ptr (const intrusive_ptr& p)
  : base (p.get () ? Strategy::clone (p.get ()) : 0)
  { }

template <class T,class Strategy> template <class T1>
inline intrusive_ptr<T,Strategy>::intrusive_ptr (const intrusive_ptr<T1,Strategy>& p)
  : base (p.get () ? Strategy::clone (p.get ()) : 0)
  { }

template <class T,class Strategy>
inline intrusive_ptr<T,Strategy>::~intrusive_ptr ()
{
  if (base::get ())
    Strategy::release (base::get ());
}

/*
    Присваивание
*/

template <class T,class Strategy>
inline intrusive_ptr<T,Strategy>& intrusive_ptr<T,Strategy>::operator = (element_type* p)
{
  if (base::get () != p)
    intrusive_ptr (p).swap (*this);

  return *this;
}

template <class T,class Strategy>
inline intrusive_ptr<T,Strategy>& intrusive_ptr<T,Strategy>::operator = (const intrusive_ptr& p)
{
  if (base::get () != p.get ())
    intrusive_ptr (p).swap (*this);

  return *this;
}

template <class T,class Strategy> template <class T1>
inline intrusive_ptr<T,Strategy>& intrusive_ptr<T,Strategy>::operator = (const intrusive_ptr<T1,Strategy>& p)
{
  if (p.get () != base::get ())
    intrusive_ptr (p).swap (*this);

  return *this;
}

/*
    Обмен значений указателей
*/

template <class T,class Strategy>
inline void swap (intrusive_ptr<T,Strategy>& p1,intrusive_ptr<T,Strategy>& p2)
{
  p1.swap (p2);
}

/*
    Приведение типов
*/

template <class T1,class T2,class Strategy>
inline intrusive_ptr<T1,Strategy> static_pointer_cast (const intrusive_ptr<T2,Strategy>& p)
{
  return static_cast<T1*> (p.get ());
}

template <class T1,class T2,class Strategy>
inline intrusive_ptr<T1,Strategy> const_pointer_cast (const intrusive_ptr<T2,Strategy>& p)
{
  return const_cast<T1*> (p.get ());
}

template <class T1,class T2,class Strategy>
inline intrusive_ptr<T1,Strategy> dynamic_pointer_cast (const intrusive_ptr<T2,Strategy>& p)
{
  return dynamic_cast<T1*> (p.get ());
}

/*
    com_intrusive_strategy
*/

template <class T>
inline T* com_intrusive_strategy<T>::clone (T* p)
{
  p->AddRef ();
  return p;
}

template <class T>
inline void com_intrusive_strategy<T>::release (T* p)
{
  p->Release ();
}

/*
    com_ptr
*/

template <class T>
inline com_ptr<T>::com_ptr ()
  : base ()
  { }

template <class T>
inline com_ptr<T>::com_ptr (element_type* p,bool addref)
  : base (p,addref)
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
inline void swap (com_ptr<T>& p1,com_ptr<T>& p2)
{
  p1.swap (p2);  
}  
