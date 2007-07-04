/*
    base pointers
*/

template <class T>
inline T* get_pointer (T* p)
{
  return p;
}

template <class T1,class T2>
inline T1* static_pointer_cast (T2* p)
{
  return static_cast<T1*> (p);
}

template <class T1,class T2>
inline T1* const_pointer_cast (T2* p)
{
  return const_cast<T1*> (p);
}

template <class T1,class T2> 
inline T1* dynamic_pointer_cast (T2* p)
{
  return dynamic_cast<T1*> (p);
}

/*
    smart_ptr_storage
*/

/*
    Конструкторы и присваивание
*/

template <class Ptr>
inline smart_ptr_storage<Ptr>::smart_ptr_storage ()
  : ptr ()
  { }
  
template <class Ptr>
inline smart_ptr_storage<Ptr>::smart_ptr_storage (element_type* in_ptr)
  : ptr (in_ptr)
  { }

template <class Ptr>
inline smart_ptr_storage<Ptr>::smart_ptr_storage (parameter_type storage)
  : ptr (storage.ptr)
  { }
  
template <class Ptr> template <class Ptr1>
inline smart_ptr_storage<Ptr>::smart_ptr_storage (const smart_ptr_storage<Ptr1>& another_ptr)
  : ptr (another_ptr.ptr)
  { }
  
template <class Ptr> template <class Ptr1>
inline smart_ptr_storage<Ptr>::smart_ptr_storage (smart_ptr_storage<Ptr1>& another_ptr)
  : ptr (another_ptr.ptr)
  { }

template <class Ptr>
inline smart_ptr_storage<Ptr>& smart_ptr_storage<Ptr>::operator = (element_type* in_ptr)
{
  ptr = in_ptr;
  return *this;
}

template <class Ptr>
inline smart_ptr_storage<Ptr>& smart_ptr_storage<Ptr>::operator = (parameter_type storage)
{
  ptr = storage.ptr;
  return *this;
}

template <class Ptr> template <class Ptr1>
inline smart_ptr_storage<Ptr>& smart_ptr_storage<Ptr>::operator = (const smart_ptr_storage<Ptr1>& another_ptr)
{
  ptr = another_ptr.ptr;
  return *this;
}

template <class Ptr> template <class Ptr1>
inline smart_ptr_storage<Ptr>& smart_ptr_storage<Ptr>::operator = (smart_ptr_storage<Ptr1>& another_ptr)
{
  ptr = another_ptr.ptr;
  return *this;
}

/*
    Доступ
*/

template <class Ptr>
inline typename smart_ptr_storage<Ptr>::element_type* smart_ptr_storage<Ptr>::get () const
{
  return get_pointer (ptr);
}

template <class Ptr>
inline typename smart_ptr_storage<Ptr>::element_type& smart_ptr_storage<Ptr>::operator * () const
{
  return *ptr;
}

template <class Ptr>
inline typename pointer_traits<Ptr>::pointer smart_ptr_storage<Ptr>::operator -> () const
{
  return ptr;
}

template <class Ptr>
inline typename smart_ptr_storage<Ptr>::element_type* get_pointer (const smart_ptr_storage<Ptr>& p)
{
  return p.get ();
}

/*
    Тестирование значения указателя
*/

template <class Ptr>
inline smart_ptr_storage<Ptr>::operator test_type* () const
{
  static int dummy = 0;

  return get_pointer (ptr) ? (test_type*)&dummy : 0;
}

/*
    Обмен значений
*/

namespace detail
{

//данная функция введена для уменьшения зависимости от STL
template <class T>
inline void swap_internal (T*& p1,T*& p2)
{
  T* tmp = p1;
  p1     = p2;
  p2     = tmp;
}

template <class T>
inline void swap_internal (T& p1,T& p2)
{
  swap (p1,p2);
}

}

template <class Ptr>
inline void smart_ptr_storage<Ptr>::swap (smart_ptr_storage& p)
{
  detail::swap_internal (p.ptr,ptr);
}

/*
    Операции сравнения
*/

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator == (const smart_ptr_storage<Ptr1>& p) const
{
  return ptr == p.ptr;
}

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator != (const smart_ptr_storage<Ptr1>& p) const
{
  return !(ptr == p.ptr);
}

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator < (const smart_ptr_storage<Ptr1>& p) const
{
  return ptr < p.ptr;
}

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator > (const smart_ptr_storage<Ptr1>& p) const
{
  return p.ptr < ptr;
}

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator <= (const smart_ptr_storage<Ptr1>& p) const
{
  return !(p.ptr < ptr);
}

template <class Ptr> template <class Ptr1>
inline bool smart_ptr_storage<Ptr>::operator >= (const smart_ptr_storage<Ptr1>& p) const
{
  return !(ptr < p.ptr);
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator == (const element_type* p) const
{
  return ptr == p;
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator != (const element_type* p) const
{
  return !(ptr == p);
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator < (const element_type* p) const
{
  return ptr < p;
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator > (const element_type* p) const
{
  return p < ptr;
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator <= (const element_type* p) const
{
  return !(p < ptr);
}

template <class Ptr>
inline bool smart_ptr_storage<Ptr>::operator >= (const element_type* p) const
{
  return !(ptr < p);
}

template <class Ptr>
inline bool operator == (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 == p2.get ();
}

template <class Ptr>
inline bool operator != (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 != p2.get ();
}

template <class Ptr>
inline bool operator < (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 < p2.get ();
}

template <class Ptr>
inline bool operator > (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 > p2.get ();
}

template <class Ptr>
inline bool operator <= (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 <= p2.get ();
}

template <class Ptr>
inline bool operator >= (const typename smart_ptr_storage<Ptr>::element_type* p1,const smart_ptr_storage<Ptr>& p2)
{
  return p1 >= p2.get ();
}
