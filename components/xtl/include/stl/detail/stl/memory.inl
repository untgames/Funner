/*
    auto_ptr
*/

template <class T>
inline auto_ptr_ref<T>::auto_ptr_ref (T* p)
  : ptr (p)
  { }
  
template <class T>
inline auto_ptr<T>::auto_ptr (element_type* p) throw ()
  : ptr (p)
  { }

template <class T>
inline auto_ptr<T>::auto_ptr (auto_ptr& x) throw ()
  : ptr (x.release ())
  { }
  
template <class T> template <class T1> 
inline auto_ptr<T>::auto_ptr (auto_ptr<T1>& x) throw ()
  : ptr (x.release ())
  { }  

template <class T>
inline auto_ptr<T>::auto_ptr (auto_ptr_ref<T> x) throw () 
  : ptr (x.ptr)
  { }

template <class T>
inline auto_ptr<T>::~auto_ptr ()
{
  delete ptr;
}

template <class T>
inline auto_ptr<T>& auto_ptr<T>::operator = (auto_ptr& x) throw ()
{
  if (&x != this)
    reset (x.release ());

  return *this;
}

template <class T> template <class T1> 
inline auto_ptr<T>& auto_ptr<T>::operator = (auto_ptr<T1>& x) throw ()
{
  if (x.get () != ptr)
    reset (x.release ());  
    
  return *this;
}

template <class T>
inline auto_ptr<T>& auto_ptr<T>::operator = (auto_ptr_ref<T> x) throw ()
{
  reset (x.ptr);  
  return *this;
}

template <class T>
inline T& auto_ptr<T>::operator * () const throw ()
{
  return *ptr;
}

template <class T>
inline T* auto_ptr<T>::operator -> () const throw ()
{
  return ptr;
}

template <class T>
inline T* auto_ptr<T>::get () const throw ()
{
  return ptr;
}

template <class T>
inline T* get_pointer (const auto_ptr<T>& p)
{
  return p.get ();
}

template <class T>
inline T* auto_ptr<T>::release () throw ()
{
  T* tmp = ptr;
  ptr    = 0;
  
  return tmp;
}

template <class T>
inline void auto_ptr<T>::reset (T* p) throw ()
{
  if (p == ptr)
    return;
    
  delete ptr;
      
  ptr = p;  
}

template <class T> template <class T1> 
inline auto_ptr<T>::operator auto_ptr_ref<T1> () throw ()
{
  return release ();
}

template <class T> template <class T1> 
inline auto_ptr<T>::operator auto_ptr<T1> () throw ()
{
  return release ();
}

/*
    raw_storage_iterator
*/

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>::raw_storage_iterator (FwdIter _iter)
  : iter (_iter)
  { }
    
template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator = (const T& obj)
{
  construct (&*iter,obj);
  return *this;
}

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator ++ ()
{
  ++iter;
  return *this;
}

template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T> raw_storage_iterator<FwdIter,T>::operator ++ (int)
{
  raw_storage_iterator tmp = *this;
  
  ++iter;
  
  return tmp;
}
    
template <class FwdIter,class T>
inline raw_storage_iterator<FwdIter,T>& raw_storage_iterator<FwdIter,T>::operator * ()
{
  return *this;
}
