/*
    Базовые функции распределения памяти 
*/

template <class T>
T* allocate (size_t count,const void* = NULL)
{
  return (T*)::operator new (sizeof (T)*count);
}

template <class T> 
inline T* try_allocate (size_t count)
{
  return (T*)::operator new (sizeof (T)*count,std::nothrow);
}

template <class T>
inline void deallocate (T* buf,size_t)
{
  if (buf)
    ::operator delete (buf);
}

/*
    Default allocator
*/

//присваивание (копирования не происходит)
template <class T> template <class Other>
inline allocator<T>& allocator<T>::operator = (const allocator<Other>&)
{
  return *this;
}

//функции общего назначения
template <class T>
inline typename allocator<T>::pointer allocator<T>::address (reference x) const
{
  return &x;
}

template <class T>
inline typename allocator<T>::const_pointer allocator<T>::address (const_reference x) const
{
  return &x;
}

template <class T>
inline typename allocator<T>::size_type allocator<T>::max_size () const throw ()
{ 
  return size_type (-1) / sizeof (T);
}

//заказ / освобождение памяти
template <class T>
inline typename allocator<T>::pointer allocator<T>::allocate (size_type count,const void* hint)
{
  return stl::allocate<T> (count,(pointer)hint);
}

template <class T>
inline void allocator<T>::deallocate (pointer ptr,size_type count)
{
  stl::deallocate (ptr,count);
}

//конструирование / разрушение объектов
template <class T>
inline void allocator<T>::construct (pointer ptr,const value_type& x)
{
  stl::construct (ptr,x);
}

template <class T>
inline void allocator<T>::destroy (pointer ptr)
{
  stl::destroy (ptr);
}

/*
    simple_allocator
*/

template <class T,class AllocFn,class FreeFn>
inline simple_allocator<T,AllocFn,FreeFn>::simple_allocator (AllocFn _alloc,FreeFn _free) throw ()
  : alloc (_alloc), free (_free)
  { }

template <class T,class AllocFn,class FreeFn>
inline simple_allocator<T,AllocFn,FreeFn>::simple_allocator (const simple_allocator& x) throw ()
  : alloc (x.alloc), free (x.free)
  { }
    
template <class T,class AllocFn,class FreeFn> template <class T1>
inline simple_allocator<T,AllocFn,FreeFn>::simple_allocator 
 (const simple_allocator<T1,AllocFn,FreeFn>& x) throw ()
    : alloc (x.alloc), free (x.free)
  { }
    
template <class T,class AllocFn,class FreeFn> template <class T1>
inline simple_allocator<T,AllocFn,FreeFn>& simple_allocator<T,AllocFn,FreeFn>::operator = 
  (const simple_allocator<T1,AllocFn,FreeFn>& x)
{
  alloc = x.alloc;
  free  = x.free;

  return *this;
}
    
template <class T,class AllocFn,class FreeFn>
inline typename simple_allocator<T,AllocFn,FreeFn>::pointer 
simple_allocator<T,AllocFn,FreeFn>::allocate (size_type count,const void*)
{
  return (pointer)alloc (count*sizeof (T));
}

template <class T,class AllocFn,class FreeFn>
inline void simple_allocator<T,AllocFn,FreeFn>::deallocate (pointer ptr,size_type)
{
  free (ptr);
}

template <class T,class AllocFn,class FreeFn> template <class T1> 
inline bool simple_allocator<T,AllocFn,FreeFn>::operator == (const simple_allocator<T1,AllocFn,FreeFn>& x) const
{
  return alloc == x.alloc && free == x.free;
}

template <class T,class AllocFn,class FreeFn> template <class T1> 
inline bool simple_allocator<T,AllocFn,FreeFn>::operator != (const simple_allocator<T1,AllocFn,FreeFn>& x) const
{
  return !(*this == x);
}
