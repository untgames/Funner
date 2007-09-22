#ifndef XTL_SUB_RANGE_HEADER
#define XTL_SUB_RANGE_HEADER

#include <xtl/iterator_range.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интервал, параметризированный по типу контейнера
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class FwdRange > 
class sub_range: public iterator_range<typename range_result_iterator<FwdRange>::type>
{
  typedef typename range_result_iterator<FwdRange>::type iterator_impl;
  typedef iterator_range<iterator_impl>                  base;
  public:
    typedef typename range_value<FwdRange>::type            value_type;
    typedef typename range_result_iterator<FwdRange>::type  iterator;
    typedef typename range_const_iterator<FwdRange>::type   const_iterator;
    typedef typename range_difference<FwdRange>::type       difference_type;
    typedef typename range_size<FwdRange>::type             size_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
                               sub_range () {}
    template <class FwdRange2> sub_range (FwdRange2&);   
    template <class FwdRange2> sub_range (const FwdRange2&);
    template <class Iter>      sub_range (Iter first, Iter last);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class FwdRange2> sub_range& operator = (FwdRange2&);
    template <class FwdRange2> sub_range& operator = (const FwdRange2&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator        begin ();
    const_iterator  begin () const;
    iterator        end   ();
    const_iterator  end   () const;
    size_type       size  () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение первого и последнего элементов интервала
///////////////////////////////////////////////////////////////////////////////////////////////////
          value_type& front ();
    const value_type& front () const;
          value_type& back  ();
    const value_type& back  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Индексный доступ (только для итераторов произвольного доступа)
///////////////////////////////////////////////////////////////////////////////////////////////////
          value_type& operator [] (size_type index);
    const value_type& operator [] (size_type index) const;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FwdRange1, class FwdRange2> bool operator == (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);
template <class FwdRange1, class FwdRange2> bool operator != (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);
template <class FwdRange1, class FwdRange2> bool operator <  (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);
template <class FwdRange1, class FwdRange2> bool operator >  (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);
template <class FwdRange1, class FwdRange2> bool operator <= (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);
template <class FwdRange1, class FwdRange2> bool operator >= (const sub_range<FwdRange1>&, const sub_range<FwdRange2>&);

#include <xtl/detail/sub_range.inl>
    
}

#endif
