/*
    Категория итератора
*/

template <class Iter> 
inline typename iterator_traits<Iter>::iterator_category iterator_category ()
{
  return typename iterator_traits<Iter>::iterator_category ();
}

/*
    Advance & distance
*/

namespace detail
{

template <class Iter, class Distance>
inline void advance_impl (Iter& i, Distance n)
{
  if (n >= 0) while (n--) ++i;
  else        while (n++) --i;
}

template <class Iter>
inline void advance_impl (Iter& i, unsigned int n)
{
  while (n--) ++i;
}

#ifdef __GNUC__

template <class Iter>
inline void advance_impl (Iter& i, unsigned long long n)
{
  while (n--) ++i;
}

#endif

template <class Iter>
inline void advance_impl (Iter& i, unsigned long n)
{
  while (n--) ++i;
}

template <class Iter>
inline void advance_impl (Iter& i, unsigned short n)
{
  while (n--) ++i;
}

template <class Iter>
inline void advance_impl (Iter& i, unsigned char n)
{
  while (n--) ++i;
}

}

template <class Iter, class Distance>
inline void advance (Iter& i, Distance n)
{
  detail::advance_impl (i, n);
}

template <class T, class Distance>
inline void advance (T*& i, Distance n)
{
  i += n;
}

template <class Iter>
inline typename iterator_traits<Iter>::difference_type distance (Iter first, Iter last) 
{
  typename iterator_traits<Iter>::difference_type n;
  
  for (n=0; first!=last; ++first, ++n);
  
  return n;
}

template <class T>
inline ptrdiff_t distance (T* first, T* last)
{
  return last - first;  
}

template <class Iter, class Distance> 
inline void distance (Iter first, Iter last, Distance& n)
{
  n = distance (first, last);
}
