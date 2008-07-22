/*
    ¬спомогательные функции
*/

namespace detail
{

//получение начала и конца интервала
template <class Iter, class ForwardRange>
inline Iter adl_range_begin (ForwardRange& r)
{
  using xtl::begin;
  return begin (r);
}

template <class Iter, class ForwardRange>
inline Iter adl_range_end (ForwardRange& r)
{
  using xtl::end;
  return end (r);
}

//сравнение двух интервалов на эквивалентность
template <class FwdRange1, class FwdRange2>
inline bool range_equal (const FwdRange1& r1, const FwdRange2& r2)
{
  typedef typename xtl::range_size<FwdRange1>::type size_type;

  size_type size1 = size (r1),
            size2 = size (r2);
            
  return size1 == size2 && stl::equal (begin (r1), end (r1), begin (r2));
}

//отношение пор€дка "меньше" дл€ двух интервалов
template <class FwdRange1, class FwdRange2>
inline bool range_less (const FwdRange1& r1, const FwdRange2& r2)
{
  return stl::lexicographical_compare (begin (r1), end (r1), begin (r2), end (r2));
}

}

/*
     онструкторы
*/

template <class FwdIter>
inline iterator_range<FwdIter>::iterator_range ()
  : first (), last (), singular (true)
  {}

template <class FwdIter> template <class FwdIter2>
inline iterator_range<FwdIter>::iterator_range (FwdIter2 in_begin, FwdIter2 in_end)
  : first (in_begin), last (in_end), singular (false)
  {}

template <class FwdIter> template <class FwdRange>
inline iterator_range<FwdIter>::iterator_range (FwdRange& r)
  : first (detail::adl_range_begin<iterator> (r)), last (detail::adl_range_end<iterator> (r)), singular (false)
  {}

template <class FwdIter> template <class FwdRange>
inline iterator_range<FwdIter>::iterator_range (const FwdRange& r)
  : first (detail::adl_range_begin<iterator> (r)), last (detail::adl_range_end<iterator> (r)), singular (false)
  {}

/*
    ѕрисваивание
*/

template <class FwdIter>
inline iterator_range<FwdIter>& iterator_range<FwdIter>::operator = (const iterator_range& r)
{
  first    = r.first;
  last     = r.last;
  singular = r.singular;

  return *this;
}

template <class FwdIter> template <class FwdIter1>
inline iterator_range<FwdIter>& iterator_range<FwdIter>::operator = (const iterator_range<FwdIter1>& r)
{
  first    = r.begin ();
  last     = r.end ();
  singular = r.empty ();

  return *this;
}

template <class FwdIter> template <class FwdRange>
inline iterator_range<FwdIter>& iterator_range<FwdIter>::operator = (FwdRange& r)
{
  first    = detail::adl_range_begin<iterator> (r);
  last     = detail::adl_range_end<iterator> (r);
  singular = false;

  return *this;
}

template <class FwdIter> template <class FwdRange>
inline iterator_range<FwdIter>& iterator_range<FwdIter>::operator = (const FwdRange& r)
{
  first    = detail::adl_range_begin<iterator> (r);
  last     = detail::adl_range_end<iterator> (r);
  singular = false;

  return *this;
}

/*
    ѕараметры интервала
*/

//начало интервала
template <class FwdIter>
inline typename iterator_range<FwdIter>::iterator iterator_range<FwdIter>::begin () const
{
  return first;
}

//конец интервала
template <class FwdIter>
inline typename iterator_range<FwdIter>::iterator iterator_range<FwdIter>::end () const
{
  return last;
}

//размер интервала
template <class FwdIter>
inline typename iterator_range<FwdIter>::size_type iterator_range<FwdIter>::size () const
{
  if (singular)
    return 0;
    
  return stl::distance (first, last);
}

//проверка на пустоту
template <class FwdIter>
inline bool iterator_range<FwdIter>::empty () const
{
  if (singular)
    return true;
    
  return first == last;
}

template <class FwdIter>
inline bool empty (const iterator_range<FwdIter>& r)
{
  return r.empty ();
}

/*
    ѕолучение первого и последнего элементов интервала
*/

template <class FwdIter>
inline typename iterator_range<FwdIter>::value_type& iterator_range<FwdIter>::front () const
{
  return *first;
}

template <class FwdIter>
inline typename iterator_range<FwdIter>::value_type& iterator_range<FwdIter>::back () const
{
  iterator tmp = last;

  return *--tmp;
}

/*
    »ндексный доступ (только дл€ итераторов произвольного доступа)
*/

template <class FwdIter>
inline typename iterator_range<FwdIter>::value_type& iterator_range<FwdIter>::operator [] (size_type index) const
{
  return first [index];
}

/*
    ѕроверка эквивалентности двух интервалов
*/

template <class FwdIter>
inline bool iterator_range<FwdIter>::equal (const iterator_range& r) const
{
  return singular == r.singular && first == r.first && last == r.last;
}

/*
    —равнение двух интервалов
*/

template <class FwdIter1, class FwdIter2>
inline bool operator == (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return detail::range_equal (r1, r2);
}

template <class FwdIter1, class FwdIter2>
inline bool operator != (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return !(r1 == r2);
}

template <class FwdIter1, class FwdIter2>
inline bool operator < (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return detail::range_less (r1, r2);
}

template <class FwdIter1, class FwdIter2>
inline bool operator > (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return r2 < r1;
}

template <class FwdIter1, class FwdIter2>
inline bool operator <= (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return !(r2 < r1);
}

template <class FwdIter1, class FwdIter2>
inline bool operator >= (const iterator_range<FwdIter1>& r1, const iterator_range<FwdIter2>& r2)
{
  return !(r1 < r2);
}

template <class FwdIter, class FwdRange>
inline bool operator == (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return detail::range_equal (r1, r2);
}

template <class FwdIter, class FwdRange>
inline bool operator != (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return !(r1 == r2);
}

template <class FwdIter, class FwdRange>
inline bool operator < (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return detail::range_less (r1, r2);
}

template <class FwdIter, class FwdRange>
inline bool operator > (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return r2 < r1;
}

template <class FwdIter, class FwdRange>
inline bool operator <= (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return !(r2 < r1);
}

template <class FwdIter, class FwdRange>
inline bool operator >= (const iterator_range<FwdIter>& r1, const FwdRange& r2)
{
  return !(r1 < r2);
}

template <class FwdRange, class FwdIter>
inline bool operator == (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return detail::range_equal (r1, r2);
}

template <class FwdRange, class FwdIter>
inline bool operator != (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return !(r1 == r2);
}

template <class FwdRange, class FwdIter>
inline bool operator < (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return detail::range_less (r1, r2);
}

template <class FwdRange, class FwdIter>
inline bool operator > (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return r2 < r1;
}

template <class FwdRange, class FwdIter>
inline bool operator <= (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return !(r2 < r1);
}

template <class FwdRange, class FwdIter>
inline bool operator >= (const FwdRange& r1, const iterator_range<FwdIter>& r2)
{
  return !(r1 < r2);
}

/*
    —оздание интервала
*/

template <class FwdIter>
inline iterator_range<FwdIter> make_iterator_range (FwdIter begin, FwdIter end)
{
  return iterator_range<FwdIter> (begin, end);
}

template <class FwdRange>
inline iterator_range<typename range_iterator<FwdRange>::type> make_iterator_range (FwdRange& r)
{
  return iterator_range<typename range_iterator<FwdRange>::type> (r);
}

template <class FwdRange>
inline iterator_range<typename range_const_iterator<FwdRange>::type> make_iterator_range (const FwdRange& r)
{
  return iterator_range<typename range_const_iterator<FwdRange>::type> (r);
}

namespace detail
{

template <class Range>
inline iterator_range<typename range_result_iterator<Range>::type> make_range_impl
 (Range&   range,
  typename range_difference<Range>::type advance_begin,
  typename range_difference<Range>::type advance_end)
{
  if (advance_begin == 0 && advance_end == 0)
    return make_iterator_range (range);

  typename range_result_iterator<Range>::type new_begin = begin (range), new_end = end (range);

  stl::advance (new_begin, advance_begin);
  stl::advance (new_end,   advance_end);
  
  return make_iterator_range (new_begin, new_end);
}

}

template <class Range>
inline iterator_range<typename range_iterator<Range>::type> make_iterator_range
 (Range&                                 range,
  typename range_difference<Range>::type advance_begin,
  typename range_difference<Range>::type advance_end)
{
  return detail::make_range_impl (range, advance_begin, advance_end);
}

template <class Range>
inline iterator_range<typename range_const_iterator<Range>::type> make_iterator_range
 (const Range&                           range,
  typename range_difference<Range>::type advance_begin,
  typename range_difference<Range>::type advance_end)
{
  return detail::make_range_impl (range, advance_begin, advance_end);
}

template <class T>
inline iterator_range<T*> make_iterator_range (size_t count, T* array)
{
  return iterator_range<T*> (array, array + count);
}

/*
    —оздание последовательности из интервала
*/

template <class Sequence, class FwdRange>
inline Sequence copy_range (const FwdRange& r)
{
  return Sequence (begin (r), end (r));
}
