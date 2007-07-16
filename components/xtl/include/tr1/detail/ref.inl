namespace detail
{

/*
    Получение адреса объекта
*/

template <class T> T* addressof (T& object)
{
  return reinterpret_cast<T*> (&const_cast<char&> (reinterpret_cast<const volatile char &> (object)));
}

}

/*
    Обёртка над ссылкой
*/

/*
    Конструктор
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
inline T& reference_wrapper<T>::get () const
{
  return *ptr;
}

/*
    Создание обёрток над ссылками
*/

template <class T>
inline reference_wrapper<T> ref (T& r)
{
  return reference_wrapper<T> (r);
}

template <class T>
inline reference_wrapper<const T> cref (const T& r)
{
  return reference_wrapper<const T> (r);
}

template <class T>
inline reference_wrapper<T> ref (reference_wrapper<T> rw)
{
  return rw;
}

template <class T>
inline reference_wrapper<const T> cref (reference_wrapper<T> rw)
{
  return rw;
}

/*
    Очистка reference_wrapper
*/

template <class T> struct remove_reference_wrapper                                       { typedef T type; };
template <class T> struct remove_reference_wrapper<reference_wrapper<T> >                { typedef T type; };
template <class T> struct remove_reference_wrapper<const reference_wrapper<T> >          { typedef T type; };
template <class T> struct remove_reference_wrapper<volatile reference_wrapper<T> >       { typedef T type; };
template <class T> struct remove_reference_wrapper<const volatile reference_wrapper<T> > { typedef T type; };
