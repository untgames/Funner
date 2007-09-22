/*
    Конструкторы
*/

template <class FwdRange> template <class FwdRange2>
inline sub_range<FwdRange>::sub_range (FwdRange2& r)
  : base (r)
  {}

template <class FwdRange> template <class FwdRange2>
inline sub_range<FwdRange>::sub_range (const FwdRange2& r)
  : base (r)
  {}

template <class FwdRange> template <class Iter>
inline sub_range<FwdRange>::sub_range (Iter first, Iter last)
  : base (first, last)
  {}

/*
    Присваивание
*/

template <class FwdRange> template <class FwdRange2>
inline sub_range<FwdRange>& sub_range<FwdRange>::operator = (FwdRange2& r)
{
  base::operator = (r);
  return *this;
}

template <class FwdRange> template <class FwdRange2>
inline sub_range<FwdRange>& sub_range<FwdRange>::operator = (const FwdRange2& r)
{
  base::operator = (r);
  return *this;
}

/*
    Параметры
*/

template <class FwdRange>
inline typename sub_range<FwdRange>::iterator sub_range<FwdRange>::begin ()
{
  return base::begin ();
}

template <class FwdRange>
inline typename sub_range<FwdRange>::const_iterator sub_range<FwdRange>::begin () const
{
  return base::begin ();
}

template <class FwdRange>
inline typename sub_range<FwdRange>::iterator sub_range<FwdRange>::end ()
{
  return base::end ();
}

template <class FwdRange>
inline typename sub_range<FwdRange>::const_iterator sub_range<FwdRange>::end () const
{
  return base::end ();
}

template <class FwdRange>
inline typename sub_range<FwdRange>::size_type sub_range<FwdRange>::size () const
{
  return base::size ();
}
    
/*
    Получение первого и последнего элементов интервала
*/

template <class FwdRange>
inline typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::front ()
{
  return base::front ();
}

template <class FwdRange>
const typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::front () const
{
  return base::front ();
}

template <class FwdRange>
inline typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::back ()
{
  return base::back ();
}

template <class FwdRange>
inline const typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::back () const
{
  return base::back ();
}

/*
    Индексный доступ (только для итераторов произвольного доступа)
*/

template <class FwdRange>
inline typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::operator [] (size_type index)
{
  return base::operator [] (index);
}

template <class FwdRange>
inline const typename sub_range<FwdRange>::value_type& sub_range<FwdRange>::operator [] (size_type index) const
{
  return base::operator [] (index);
}

/*
    Сравнение
*/

template <class FwdRange1, class FwdRange2>
inline bool operator == (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return detail::range_equal (r1, r2);
}

template <class FwdRange1, class FwdRange2>
inline bool operator != (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return !(r1 == r2);
}

template <class FwdRange1, class FwdRange2>
inline bool operator < (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return detail::range_less (r1, r2);
}

template <class FwdRange1, class FwdRange2>
inline bool operator > (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return r2 < r1;
}

template <class FwdRange1, class FwdRange2>
inline bool operator <= (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return !(r2 < r1);
}

template <class FwdRange1, class FwdRange2>
inline bool operator >= (const sub_range<FwdRange1>& r1, const sub_range<FwdRange2>& r2)
{
  return !(r1 < r2);
}
