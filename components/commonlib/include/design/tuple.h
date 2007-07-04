#ifndef DESIGN_TUPLE_HEADER
#define DESIGN_TUPLE_HEADER

#include <design/typelist.h>

//доделать конструкторы!!

namespace design
{

//implementation forwards
template <class T> class reference_wrapper;

namespace tuples
{

using type_manip::null_type;

namespace detail
{

template <size_t count>       struct element_base;
template <class T>            struct access_traits;
template <class T,class Base> class element;
template <class T0=null_type,class T1=null_type,class T2=null_type,class T3=null_type,class T4=null_type,
          class T5=null_type,class T6=null_type,class T7=null_type,class T8=null_type,class T9=null_type>
struct make_tuple_mapper;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кортеж
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0=null_type,class T1=null_type,class T2=null_type,class T3=null_type,class T4=null_type,
          class T5=null_type,class T6=null_type,class T7=null_type,class T8=null_type,class T9=null_type>
class tuple
{
  template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
  friend class tuple;

  typedef typename detail::access_traits<T0>::const_type param0_type;
  typedef typename detail::access_traits<T1>::const_type param1_type;
  typedef typename detail::access_traits<T2>::const_type param2_type;
  typedef typename detail::access_traits<T3>::const_type param3_type;
  typedef typename detail::access_traits<T4>::const_type param4_type;
  typedef typename detail::access_traits<T5>::const_type param5_type;
  typedef typename detail::access_traits<T6>::const_type param6_type;
  typedef typename detail::access_traits<T7>::const_type param7_type;
  typedef typename detail::access_traits<T8>::const_type param8_type;
  typedef typename detail::access_traits<T9>::const_type param9_type;

  public:  
    typedef typename type_manip::list<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::result elements_list;

    template <size_t number> struct element
    {
      typedef typename type_manip::at<elements_list,number>::result type;
      typedef typename detail::access_traits<type>::const_type      const_type;
      typedef typename detail::access_traits<type>::non_const_type  non_const_type;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    tuple () {}    
    tuple (param0_type);
    tuple (param0_type,param1_type);
    tuple (param0_type,param1_type,param2_type);
    tuple (param0_type,param1_type,param2_type,param3_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type,param5_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type,param5_type,param6_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type,param5_type,param6_type,param7_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type,param5_type,param6_type,param7_type,param8_type);
    tuple (param0_type,param1_type,param2_type,param3_type,param4_type,param5_type,param6_type,param7_type,param8_type,param9_type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    tuple& operator = (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выбор элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <size_t number> typename element<number>::const_type     get () const;
    template <size_t number> typename element<number>::non_const_type get ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение (в лексикографическом порядке)
///////////////////////////////////////////////////////////////////////////////////////////////////
/*    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator == (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;

    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator != (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;

    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator < (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;

    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator <= (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;

    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator > (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;

    template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    bool operator >= (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>&) const;*/

  private:    
    typedef typename type_manip::reverse<elements_list>::result reverse_elements_list;
    typedef typename type_manip::generate_linear_hierarchy<reverse_elements_list,detail::element,
                     detail::element_base<type_manip::length<elements_list>::result> >::result holder_type;

    holder_type elements;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Селекторы элементов кортежа (пример использования get<0>(t))
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t number,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::const_type
get (const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

template <size_t number,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::non_const_type
get (tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание кортежей
///////////////////////////////////////////////////////////////////////////////////////////////////
tuple<> make_tuple ();

template <class T0>
typename detail::make_tuple_mapper<T0>::type make_tuple (const T0&);

template <class T0,class T1>
typename detail::make_tuple_mapper<T0,T1>::type make_tuple (const T0&,const T1&);

template <class T0,class T1,class T2>
typename detail::make_tuple_mapper<T0,T1,T2>::type make_tuple (const T0&,const T1&,const T2&);

template <class T0,class T1,class T2,class T3>
typename detail::make_tuple_mapper<T0,T1,T2,T3>::type make_tuple (const T0&,const T1&,const T2&,const T3&);

template <class T0,class T1,class T2,class T3,class T4>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&);

template <class T0,class T1,class T2,class T3,class T4,class T5>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&,const T5&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&,const T5&,const T6&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&,const T5&,const T6&,const T7&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&,const T5&,const T6&,const T7&,const T8&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8>::type
make_tuple (const T0&,const T1&,const T2&,const T3&,const T4&,const T5&,const T6&,const T7&,const T8&,const T9&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ссылочных кортежей (все элементы являются ссылками)
///////////////////////////////////////////////////////////////////////////////////////////////////
tuple<> tie ();

template <class T0>
tuple<T0&> tie (T0&);

template <class T0,class T1>
tuple<T0&,T1&> tie (T0&,T1&);

template <class T0,class T1,class T2>
tuple<T0&,T1&,T2&> tie (T0&,T1&,T2&);

template <class T0,class T1,class T2,class T3>
tuple<T0&,T1&,T2&,T3&> tie (T0&,T1&,T2&,T3&);

template <class T0,class T1,class T2,class T3,class T4>
tuple<T0&,T1&,T2&,T3&,T4&> tie (T0&,T1&,T2&,T3&,T4&);

template <class T0,class T1,class T2,class T3,class T4,class T5>
tuple<T0&,T1&,T2&,T3&,T4&,T5&> tie (T0&,T1&,T2&,T3&,T4&,T5&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6>
tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&> tie (T0&,T1&,T2&,T3&,T4&,T5&,T6&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&> tie (T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&> tie (T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&);

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&> tie (T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&,T9&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Применение функтора к списку аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
Ret apply (Fn,const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
Ret apply (Fn,tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
Ret apply (const reference_wrapper<Fn>&,const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
Ret apply (const reference_wrapper<Fn>&,tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>&);

#include <design/impl/tuple.inl>

}

using tuples::tuple;
using tuples::get;
using tuples::make_tuple;
using tuples::tie;

}

#endif
