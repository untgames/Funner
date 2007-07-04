namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица преобразования типов элементов кортежа при конструировании
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct access_traits
{
  typedef T        type;
  typedef const T& const_type;
  typedef T&       non_const_type;
};

template <class T> struct access_traits<T&>
{
  typedef T  type;
  typedef T& const_type;
  typedef T& non_const_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица допустимых типов элементов кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct make_tuple_traits
{
  typedef T type;
};

template <class T,size_t size> struct make_tuple_traits<T [size]>
{
  typedef const T (&type)[size];
};

template <class T,size_t size> struct make_tuple_traits<const T [size]>
{
  typedef const T (&type)[size];
};

template <class T,size_t size> struct make_tuple_traits<volatile T [size]>
{
  typedef const volatile T (&type)[size];
};

template <class T,size_t size> struct make_tuple_traits<const volatile T [size]>
{
  typedef const volatile T (&type)[size];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа кортежа по типу его элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
struct make_tuple_mapper
{
  typedef tuple<typename make_tuple_traits<T0>::type,
                typename make_tuple_traits<T1>::type,
                typename make_tuple_traits<T2>::type,
                typename make_tuple_traits<T3>::type,
                typename make_tuple_traits<T4>::type,
                typename make_tuple_traits<T5>::type,
                typename make_tuple_traits<T6>::type,
                typename make_tuple_traits<T7>::type,
                typename make_tuple_traits<T8>::type,
                typename make_tuple_traits<T9>::type> type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данная структура используется для выбора элемента кортежа по номеру
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t number> struct element_selector {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Корень иерархии классов описывающих элементы кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t elements_count>
struct element_base
{
  enum { element_number = elements_count };
  
  element_base () {}
  element_base (const null_type&,const null_type&,const null_type&,const null_type&,const null_type&,
                const null_type&,const null_type&,const null_type&,const null_type&,const null_type&) {}
                
  template <size_t number>
  void get (element_selector<number>) const; //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс хранящий элемент кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct element_holder //non_storable_type
{
  element_holder () : value () {}
  element_holder (typename access_traits<T>::const_type in_value) : value (in_value) {}

  T value;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Элемент кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Base,class T>
class element: public Base
{
  public:
    enum { element_number = Base::element_number-1 };

    element () {}

    template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    element (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5,T6& t6,T7& t7,T8& t8,T9& t9) :
      holder (t0), Base (t1,t2,t3,t4,t5,t6,t7,t8,t9,null_type ()) {}

    typename access_traits<T>::const_type get (element_selector<element_number>) const
    { 
      return holder.value;
    }

    typename access_traits<T>::non_const_type get (element_selector<element_number>)
    { 
      return holder.value;
    }

    using Base::get;
    
  private:
    element_holder<T> holder;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательный класс специализирующий вызовы функторов для разного набора аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t arguments_count>
struct functor_applier;

template <> struct functor_applier<0>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple&)
  {
    return fn ();
  }
};

template <> struct functor_applier<1>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t));
  }
};

template <> struct functor_applier<2>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t));
  }
};

template <> struct functor_applier<3>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t));
  }
};

template <> struct functor_applier<4>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t));
  }
};

template <> struct functor_applier<5>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t),get<4> (t));
  }
};

template <> struct functor_applier<6>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t),get<4> (t),get<5> (t));
  }
};

template <> struct functor_applier<7>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t),get<4> (t),get<5> (t),get<6> (t));
  }
};

template <> struct functor_applier<8>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t),get<4> (t),get<5> (t),get<6> (t),get<7> (t));
  }
};

template <> struct functor_applier<9>
{
  template <class Ret,class Fn,class Tuple> static Ret apply (Fn& fn,Tuple& t)
  {
    return fn (get<0> (t),get<1> (t),get<2> (t),get<3> (t),get<4> (t),get<5> (t),get<6> (t),get<7> (t),get<9> (t));
  }
};

}

/*
    tuple
*/

/*
    Конструкторы
*/

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple (param0_type p0)
  : elements (p0,null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type ())
  { }

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple (param0_type p0,param1_type p1)
  : elements (p0,p1,null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type ())
  { }

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple (param0_type p0,param1_type p1,param2_type p2)
  : elements (p0,p1,p2,null_type (),null_type (),null_type (),null_type (),null_type (),null_type (),null_type ())
  { }

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple (param0_type p0,param1_type p1,param2_type p2,param3_type p3)
  : elements (p0,p1,p2,p3,null_type (),null_type (),null_type (),null_type (),null_type (),null_type ())
  { }
  
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4) 
    : elements (p0,p1,p2,p3,p4,null_type (),null_type (),null_type (),null_type (),null_type ())
  { }
  
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4,
  param5_type p5)
    : elements (p0,p1,p2,p3,p4,p5,null_type (),null_type (),null_type (),null_type ())
  { }

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4,
  param5_type p5,
  param6_type p6)
    : elements (p0,p1,p2,p3,p4,p5,p6,null_type (),null_type (),null_type ())
  { }
  
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4,
  param5_type p5,
  param6_type p6,
  param7_type p7)
    : elements (p0,p1,p2,p3,p4,p5,p6,p7,null_type (),null_type ())
  { }
  
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4,
  param5_type p5,
  param6_type p6,
  param7_type p7,
  param8_type p8)
    : elements (p0,p1,p2,p3,p4,p5,p6,p7,p8,null_type ())
  { }
  
template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::tuple
 (param0_type p0,
  param1_type p1,
  param2_type p2,
  param3_type p3,
  param4_type p4,
  param5_type p5,
  param6_type p6,
  param7_type p7,
  param8_type p8,
  param9_type p9)
    : elements (p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
  { }

/*
    Присваивание
*/

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
template <class A0,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
inline tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::operator =
 (const tuple<A0,A1,A2,A3,A4,A5,A6,A7,A8,A9>& t)
{
  elements = t.elements;

  return *this;
}

/*
    Выборка
*/

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9> template <size_t number>
inline typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::const_type
tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::get () const
{
  return elements.get (detail::element_selector<number> ());
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9> template <size_t number>
inline typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::non_const_type
tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::get ()
{
  return elements.get (detail::element_selector<number> ());
}

template <size_t number,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::const_type get
 (const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  return t.get<number> ();
}

template <size_t number,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline typename tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::template element<number>::non_const_type get
 (tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  return t.get<number> ();
}

/*
    Создание кортежей
*/

inline tuple<> make_tuple ()
{
  return tuple<> ();
}

template <class T0>
inline typename detail::make_tuple_mapper<T0>::type make_tuple (const T0& t0)
{
  return typename detail::make_tuple_mapper<T0>::type (t0);
}

template <class T0,class T1>
inline typename detail::make_tuple_mapper<T0,T1>::type make_tuple (const T0& t0,const T1& t1)
{
  return typename detail::make_tuple_mapper<T0,T1>::type (t0,t1);
}

template <class T0,class T1,class T2>
inline typename detail::make_tuple_mapper<T0,T1,T2>::type make_tuple (const T0& t0,const T1& t1,const T2& t2)
{
  return typename detail::make_tuple_mapper<T0,T1,T2>::type (t0,t1,t2);
}

template <class T0,class T1,class T2,class T3>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3>::type (t0,t1,t2,t3);
}

template <class T0,class T1,class T2,class T3,class T4>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4>::type (t0,t1,t2,t3,t4);
}

template <class T0,class T1,class T2,class T3,class T4,class T5>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4,
  const T5& t5)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5>::type (t0,t1,t2,t3,t4,t5);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4,
  const T5& t5,
  const T6& t6)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6>::type (t0,t1,t2,t3,t4,t5,t6);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4,
  const T5& t5,
  const T6& t6,
  const T7& t7)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7>::type (t0,t1,t2,t3,t4,t5,t6,t7);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4,
  const T5& t5,
  const T6& t6,
  const T7& t7,
  const T8& t8)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8>::type (t0,t1,t2,t3,t4,t5,t6,t7,t8);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::type make_tuple
 (const T0& t0,
  const T1& t1,
  const T2& t2,
  const T3& t3,
  const T4& t4,
  const T5& t5,
  const T6& t6,
  const T7& t7,
  const T8& t8,
  const T9& t9)
{
  return typename detail::make_tuple_mapper<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::type (t0,t1,t2,t3,t4,t5,t6,t7,t8,t9);
}

/*
    Создание ссылочных кортежей
*/

inline tuple<> tie ()
{
  return tuple<> ();
}

template <class T0>
inline tuple<T0&> tie (T0& t0)
{
  return tuple<T0&> (t0);
}

template <class T0,class T1>
inline tuple<T0&,T1&> tie (T0& t0,T1& t1)
{
  return tuple<T0&,T1&> (t0&,T1);
}

template <class T0,class T1,class T2>
inline tuple<T0&,T1&,T2&> tie (T0& t0,T1& t1,T2& t2)
{
  return tuple<T0&,T1&,T2&> (t0&,T1&,T2);
}

template <class T0,class T1,class T2,class T3>
inline tuple<T0&,T1&,T2&,T3&> tie (T0& t0,T1& t1,T2& t2,T3& t3)
{
  return tuple<T0&,T1&,T2&,T3&> (t0&,T1&,T2&,T3);
}

template <class T0,class T1,class T2,class T3,class T4>
inline tuple<T0&,T1&,T2&,T3&,T4&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4)
{
  return tuple<T0&,T1&,T2&,T3&,T4&> (t0&,T1&,T2&,T3&,T4);
}

template <class T0,class T1,class T2,class T3,class T4,class T5>
inline tuple<T0&,T1&,T2&,T3&,T4&,T5&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5)
{
  return tuple<T0&,T1&,T2&,T3&,T4&,T5&> (t0&,T1&,T2&,T3&,T4&,T5);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6>
inline tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5,T6& t6)
{
  return tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&> (t0&,T1&,T2&,T3&,T4&,T5&,T6);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
inline tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5,T6& t6,T7& t7)
{
  return tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&> (t0&,T1&,T2&,T3&,T4&,T5&,T6&,T7);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
inline tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5,T6& t6,T7& t7,T8& t8)
{
  return tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&> (t0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8);
}

template <class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&,T9&> tie (T0& t0,T1& t1,T2& t2,T3& t3,T4& t4,T5& t5,T6& t6,T7& t7,T8& t8,T9& t9)
{
  return tuple<T0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&,T9&> (t0&,T1&,T2&,T3&,T4&,T5&,T6&,T7&,T8&,T9);
}

/*
    Применение функтора к списку аргументов
*/

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline Ret apply (Fn fn,const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  typedef tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::elements_list arguments_list;

  return detail::functor_applier<type_manip::length<arguments_list>::result>::apply<Ret> (fn,t);
}

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline Ret apply (Fn fn,tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  typedef tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::elements_list arguments_list;

  return detail::functor_applier<type_manip::length<arguments_list>::result>::apply<Ret> (fn,t);
}

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline Ret apply (const reference_wrapper<Fn>& fn,const tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  typedef tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::elements_list arguments_list;

  return detail::functor_applier<type_manip::length<arguments_list>::result>::apply<Ret> (get_reference (fn),t);
}

template <class Ret,class Fn,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
inline Ret apply (const reference_wrapper<Fn>& fn,tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>& t)
{
  typedef tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::elements_list arguments_list;

  return detail::functor_applier<type_manip::length<arguments_list>::result>::apply<Ret> (get_reference (fn),t);
}
