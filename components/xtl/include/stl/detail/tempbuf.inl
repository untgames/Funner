/*
    База
*/

template <class T> 
inline pair<T*,size_t> get_temporary_buffer (size_t len)
{
  if (len > size_t (-1) / sizeof(T))
    len = size_t (-1) / sizeof (T);
    
  T* buf;  

  for (buf=NULL;!buf && len;len/=2)
    buf = try_allocate<T> (len);
    
  return pair<T*,size_t> (buf,len);
}

template <class T> 
inline void return_temporary_buffer (T* buffer)
{
  deallocate (buffer,0);
}

/*
    Конструкторы / деструктор
*/

template <class T>
temporary_buffer<T>::temporary_buffer (size_type request_size)
{
  pair<T*,size_t> tmp = get_temporary_buffer<T> (request_size);
  
  buffer = tmp.first;
  len    = tmp.second;
  requested_len = request_size;  
  
  try
  {    
    if (tmp.second)
      new (tmp.first) T [tmp.second];
  }
  catch (...)
  {
    buffer = NULL;
    len    = 0;  
    return_temporary_buffer (tmp.first);
    throw;
  }
}

template <class T>
temporary_buffer<T>::temporary_buffer (size_type request_size,const T& val)
{
  pair<T*,size_t> tmp = get_temporary_buffer<T> (request_size);
  
  buffer = tmp.first;
  len    = tmp.second;
  requested_len = request_size;  
  
  try
  {    
    if (tmp.second)
      uninitialized_fill_n (tmp.first,tmp.second,val);
  }
  catch (...)
  {
    buffer = NULL;
    len    = 0;  
    return_temporary_buffer (tmp.first);
    throw;
  }
}

template <class T>
inline temporary_buffer<T>::~temporary_buffer ()
{
  destroy (begin (),end ());
  return_temporary_buffer (buffer);
}

/*
    Размер / запрошенный размер 
*/

template <class T>
inline typename temporary_buffer<T>::size_type temporary_buffer<T>::size () const
{
  return len;
}

template <class T>
inline typename temporary_buffer<T>::size_type temporary_buffer<T>::requested_size () const
{
  return requested_len;
}
    
/*
    Получение итераторов
*/

template <class T>
inline typename temporary_buffer<T>::pointer temporary_buffer<T>::begin ()
{
  return buffer;
}

template <class T>
inline typename temporary_buffer<T>::pointer temporary_buffer<T>::end ()
{
  return buffer+len;
}

template <class T>
inline typename temporary_buffer<T>::const_pointer temporary_buffer<T>::begin () const
{
  return buffer;
}

template <class T> 
inline typename temporary_buffer<T>::const_pointer temporary_buffer<T>::end () const
{
  return buffer+len;
}
