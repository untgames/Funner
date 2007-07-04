/*
    Получение адреса объекта
*/

template <class T> T* addressof (T& object)
{
  return reinterpret_cast<T*> (&const_cast<char&> (reinterpret_cast<const volatile char &> (object)));
}

/*
    Обёртка над ссылкой
*/

template <class T>
inline reference_wrapper<T>::reference_wrapper (T& ref)
  : ptr (addressof (ref))
  { }
  
template <class T>
inline reference_wrapper<T>::operator T& () const
{
  return *ptr;
}

template <class T>
inline T& reference_wrapper<T>::get_reference () const
{
  return *ptr;
}

template <class T>
inline T* reference_wrapper<T>::get_pointer () const
{
  return ptr;
}

template <class T>
inline T& get_reference (const reference_wrapper<T>& ref)
{
  return ref.get_reference ();
}

template <class T>
inline T* get_pointer (const reference_wrapper<T>& ref)
{
  return ref.get_pointer ();
}

template <class T>
inline const reference_wrapper<T> ref (T& ref)
{
  return reference_wrapper<T> (ref);
}

template <class T>
inline const reference_wrapper<T const> cref (T const& ref)
{
  return reference_wrapper<T const> (ref);
}
