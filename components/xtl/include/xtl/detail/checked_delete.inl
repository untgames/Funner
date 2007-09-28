template <class T>
inline void checked_delete (T* ptr)
{
  typedef char type_must_be_complete [sizeof (T) ? 1: -1];
  (void)sizeof (type_must_be_complete);

  delete ptr;
}

template <class T>
inline void checked_array_delete (T* ptr)
{
  typedef char type_must_be_complete [sizeof (T) ? 1: -1];
  
  (void)sizeof (type_must_be_complete);

  delete [] ptr;
}
