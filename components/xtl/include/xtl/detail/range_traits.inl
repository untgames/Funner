/*
    Метафункции для однопроходных интервалов
*/

//тип значения элементов интервала
template <class T> struct range_value
{
  typedef typename stl::iterator_traits<typename range_iterator<T>::type>::value_type type;
};

//тип итератора интервала
template <class C>               struct range_iterator                                       { typedef typename C::iterator type; };
template <class Iterator>        struct range_iterator<stl::pair<Iterator, Iterator> >       { typedef Iterator type; };
template <class Iterator>        struct range_iterator<const stl::pair<Iterator, Iterator> > { typedef Iterator type; };
template <class T, size_t Size>  struct range_iterator<T [Size]>                             { typedef T* type; };
template <class T, size_t Size>  struct range_iterator<const T [Size]>                       { typedef const T* type; };
template <class T, size_t Size>  struct range_iterator<volatile T [Size]>                    { typedef volatile T* type; };
template <class T, size_t Size>  struct range_iterator<const volatile T [Size]>              { typedef const volatile T* type; };

//тип константного итератора интервала
template <class C>               struct range_const_iterator                                       { typedef typename C::const_iterator type; };
template <class Iterator>        struct range_const_iterator<stl::pair<Iterator, Iterator> >       { typedef Iterator type; };
template <class Iterator>        struct range_const_iterator<const stl::pair<Iterator, Iterator> > { typedef Iterator type; };
template <class T, size_t Size>  struct range_const_iterator<T [Size]>                             { typedef const T* type; };
template <class T, size_t Size>  struct range_const_iterator<const T [Size]>                       { typedef const T* type; };
template <class T, size_t Size>  struct range_const_iterator<volatile T [Size]>                    { typedef const T* type; };
template <class T, size_t Size>  struct range_const_iterator<const volatile T [Size]>              { typedef const T* type; };

/*
    Метафункции для интервалов прямого перебора
*/

//тип разности между двумя итераторами интервала
template <class T> struct range_difference
{
  typedef typename stl::iterator_traits<typename range_const_iterator<T>::type>::difference_type type;
};

//тип размерности интервала
template <class C>               struct range_size                                      { typedef typename C::size_type type; };
template <class Iterator>        struct range_size<stl::pair<Iterator, Iterator> >      { typedef size_t type; };
template <class Iterator>        struct range_size<const stl::pair<Iterator,Iterator> > { typedef size_t type; };
template <class T, size_t Size>  struct range_size<T [Size]>                            { typedef size_t type; };
template <class T, size_t Size>  struct range_size<const T [Size]>                      { typedef size_t type; };
template <class T, size_t Size>  struct range_size<volatile T [Size]>                   { typedef size_t type; };
template <class T, size_t Size>  struct range_size<const volatile T [Size]>             { typedef size_t type; };

/*
    Метафункции для интервалов двунаправленного перебора
*/

//определение типа итератора обратного перебора
template <class T> struct range_reverse_iterator
{
  typedef stl::reverse_iterator<typename range_iterator<T>::type> type;
};

//определение типа константного итератора обратного перебора
template <class T> struct range_const_reverse_iterator
{
  typedef stl::reverse_iterator<typename range_const_iterator<T>::type> type;
};

/*
    Дополнительные метафункции
*/

template <class T> struct range_result_iterator
{  
  typedef typename range_iterator<T>::type type;
};

template <class T> struct range_result_iterator<const T>
{  
  typedef typename range_const_iterator<const T>::type type;
};

template <class T> struct range_result_iterator<const volatile T>
{  
  typedef typename range_const_iterator<const volatile T>::type type;
};

template <class T, size_t Size> struct range_result_iterator<const T [Size]>
{  
  typedef typename range_const_iterator<const T [Size]>::type type;
};

template <class T, size_t Size> struct range_result_iterator<const volatile T [Size]>
{  
  typedef typename range_const_iterator<const volatile T [Size]>::type type;
};

template <class T> struct range_reverse_result_iterator
{
  typedef stl::reverse_iterator<typename range_result_iterator<T>::type> type;
};

/*
    Функции для однопроходных интервалов перебора
*/

//получение итератора на начало интервала
template <class T>
inline typename range_const_iterator<T>::type begin (const T& c)
{
  return c.begin ();
}

template <class T>
inline typename range_iterator<T>::type begin (T& c)
{
  return c.begin ();
}

template <class Iterator>
inline Iterator begin (const stl::pair<Iterator, Iterator>& p)
{
  return p.first;
}

template <class Iterator>
inline Iterator begin (stl::pair<Iterator, Iterator>& p)
{
  return p.first;
}

template <class T, size_t Size>
inline const T* begin (const T (&array)[Size])
{
  return array;
}

template <class T, size_t Size>
inline T* begin (T (&array)[Size])
{
  return array;
}

template <class T>
inline typename range_const_iterator<T>::type const_begin (const T& c)
{
  return begin (c);
}

//получение итератора на конец интервала
template <class T>
inline typename range_const_iterator<T>::type end (const T& c)
{
  return c.end ();
}

template <class T>
inline typename range_iterator<T>::type end (T& c)
{
  return c.end ();
}

template <class Iterator>
inline Iterator end (const stl::pair<Iterator, Iterator>& p)
{
  return p.second;
}

template <class Iterator>
inline Iterator end (stl::pair<Iterator, Iterator>& p)
{
  return p.second;
}

template <class T, size_t Size>
inline const T* end (const T (&array)[Size])
{
  return array + Size;
}

template <class T, size_t Size>
inline T* end (T (&array)[Size])
{
  return array + Size;
}

template <class T>
inline typename range_const_iterator<T>::type const_end (const T& c)
{
  return end (c);
}

//проверка на пустоту интервала
template <class T>
inline bool empty (const T& c)
{
  return xtl::begin (c) == xtl::end (c);
}

/*
    Функции для интервалов прямого перебора
*/

//определение количества элементов в интервале
template <class T> inline typename range_size<T>::type size (const T& c)
{
  return c.size ();
}

template <class Iterator> inline size_t size (const stl::pair<Iterator, Iterator>& p)
{
  return stl::distance (p.first, p.second);
}

template <class T, size_t Size> inline size_t size (const T (&array)[Size])
{
  return Size;
}

/*
    Функции для интервалов двунаправленного перебора
*/

//получение обратного итератора на начало интервала
template <class T>
inline typename range_reverse_result_iterator<T>::type rbegin (T& c)
{
  return typename range_reverse_result_iterator<T>::type (end (c));
}

template <class T>
inline typename range_const_reverse_iterator<T>::type const_rbegin (const T& c)
{
  return rbegin (c);
}

//получение обратного итератора на конец интервала
template <class T> typename range_reverse_result_iterator<T>::type rend (T& c)
{
  return typename range_reverse_result_iterator<T>::type (begin (c));
}

template <class T>
inline typename range_const_reverse_iterator<T>::type const_rend (const T& c)
{
  return rend (c);
}
