/*
    shared_ptr
*/

/*
    Конструкторы / декструктор
*/

template <class T,class Control>
inline shared_ptr<T,Control>::shared_ptr ()
  : count ()
  { }

template <class T,class Control>
inline shared_ptr<T,Control>::shared_ptr (element_type* p)
  : base (p), count (new size_t (1))
  { }

template <class T,class Control>
inline shared_ptr<T,Control>::shared_ptr (const shared_ptr& p)
  : base (p), count (p.count)
{ 
  (*count)++;
}

template <class T,class Control> template <class T1>
inline shared_ptr<T,Control>::shared_ptr (const shared_ptr<T1,Control>& p)
  : base (p), count (p.count)
{ 
  (*count)++;
}

template <class T,class Control> template <class T1>
inline shared_ptr<T,Control>::shared_ptr (const shared_ptr<T1,Control>& p,static_cast_tag)
  : base (static_cast<T*> (p.get ())), count (p.count)
{ 
  (*count)++;
}

template <class T,class Control> template <class T1>
inline shared_ptr<T,Control>::shared_ptr (const shared_ptr<T1,Control>& p,const_cast_tag)
  : base (const_cast<T*> (p.get ())), count (p.count)
{ 
  (*count)++;
}

template <class T,class Control> template <class T1>
inline shared_ptr<T,Control>::shared_ptr (const shared_ptr<T1,Control>& p,dynamic_cast_tag)
  : base (dynamic_cast<T*> (p.get ())), count (p.count)
{ 
  (*count)++;
}

template <class T,class Control>
inline shared_ptr<T,Control>::~shared_ptr ()
{
  if (count && !--(*count))
  {
    Control::destroy (base::get ());
    delete count;
  }
}

/*
    Присваивание
*/

template <class T,class Control>
inline shared_ptr<T,Control>& shared_ptr<T,Control>::operator = (element_type* p)
{
  if (p != base::get ())
    shared_ptr (p).swap (*this);

  return *this;
}

template <class T,class Control>
inline shared_ptr<T,Control>& shared_ptr<T,Control>::operator = (const shared_ptr& p)
{
  if (base::get () != p.get ())
    shared_ptr (p).swap (*this);

  return *this;
}

template <class T,class Control> template <class T1>
inline shared_ptr<T,Control>& shared_ptr<T,Control>::operator = (const shared_ptr<T1,Control>& p)
{
  if (p != base::get ())
    shared_ptr (p).swap (*this);

  return *this;
}

/*
    Обмен значений указателей
*/

template <class T,class Control>
inline void shared_ptr<T,Control>::swap (shared_ptr& p)
{
  base::swap (p);
  detail::swap_internal (count,p.count);
}

template <class T,class Control>
inline void swap (shared_ptr<T,Control>& p1,shared_ptr<T,Control>& p2)
{
  p1.swap (p2);
}

/*
    Приведение типов
*/

template <class T1,class T2,class Control>
inline shared_ptr<T1,Control> static_pointer_cast (const shared_ptr<T2,Control>& p)
{
  return shared_ptr<T1,Control> (p,static_cast_tag ());
}

template <class T1,class T2,class Control>
inline shared_ptr<T1,Control> const_pointer_cast (const shared_ptr<T2,Control>& p)
{
  return shared_ptr<T1,Control> (p,const_cast_tag ());
}

template <class T1,class T2,class Control>
inline shared_ptr<T1,Control> dynamic_pointer_cast (const shared_ptr<T2,Control>& p)
{
  return shared_ptr<T1,Control> (p,dynamic_cast_tag ());
}

/*
    default_object_deleter
*/

template <class T>
inline void default_object_deleter<T>::destroy (T* p)
{
  delete p;
}
