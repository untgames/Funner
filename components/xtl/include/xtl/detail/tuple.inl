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

template <> struct access_traits<mpl::null_type>
{
  typedef mpl::null_type type, const_type, non_const_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс хранящий элемент кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Tail> class tuple_element_holder
{
  T value;
  public:
    tuple_element_holder () : value () {}
    tuple_element_holder (const tuple_element_holder& h) : value (h.value) {}

    template <class T1>
    explicit tuple_element_holder (T1& in_value) : value (in_value) {}

    tuple_element_holder& operator = (const tuple_element_holder& h)
    {
      value = h.in_value;
      return *this;
    }

    template <class T1>
    tuple_element_holder& operator = (T1& in_value)
    {
      value = in_value;
      return *this;
    }

    typename access_traits<T>::const_type     get () const { return value; }
    typename access_traits<T>::non_const_type get ()       { return value; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Cons-список
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Head, class Tail>
class cons: private tuple_element_holder<Head, Tail>, public Tail
{
  typedef tuple_element_holder<Head, Tail>             holder_type;
  typedef Tail                                         tail_type;
  typedef typename access_traits<Head>::const_type     const_head_type;
  typedef typename access_traits<Head>::non_const_type head_type;
  public:
    enum { tuple_size = tail_type::tuple_size + 1 };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    cons () {}

    template <class T0>
    cons (T0& t0, mpl::null_type) :
      holder_type (t0) {}

    template <class T0, class T1>
    cons (T0& t0, T1& t1) :
      holder_type (t0), tail_type (t1, mpl::null_type ()) {}
    
    template <class T0, class T1, class T2>
    cons (T0& t0, T1& t1, T2& t2) :
      holder_type (t0), tail_type (t1, t2) {}

    template <class T0, class T1, class T2, class T3>
    cons (T0& t0, T1& t1, T2& t2, T3& t3) :
      holder_type (t0), tail_type (t1, t2, t3) {}

    template <class T0, class T1, class T2, class T3, class T4>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4) :
      holder_type (t0), tail_type (t1, t2, t3, t4) {}

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) :
      holder_type (t0), tail_type (t1, t2, t3, t4, t5) {}

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6) :
      holder_type (t0), tail_type (t1, t2, t3, t4, t5, t6) {}

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7) :
      holder_type (t0), tail_type (t1, t2, t3, t4, t5, t6, t7) {}

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8) :
      holder_type (t0), tail_type (t1, t2, t3, t4, t5, t6, t7, t8) {}

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    cons (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9) :
      holder_type (t0), tail_type (t1, t2, t3, t4, t5, t6, t7, t8, t9) {}

    template <class Head1, class Tail1>
    cons (const cons<Head1, Tail1>& t) :
      holder_type (t.get_head ()), tail_type (t.get_tail ()) {}

    cons (const cons& t) :
      holder_type (t.get_head ()), tail_type (t.get_tail ()) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Head1, class Tail1> cons& operator = (const cons<Head1, Tail1>& t)
    {        
      holder_type::operator = (t.get_head ());
      tail_type::operator = (t.get_tail ());

      return *this;
    }
    
    cons& operator = (const cons& t)
    {        
      holder_type::operator = (t.get_head ());
      tail_type::operator = (t.get_tail ());

      return *this;
    }
    
    void assign () {}

    template <class T0>
    void assign (T0& t0)
    {
      holder_type::operator = (t0);
    }

    template <class T0, class T1>
    void assign (T0& t0, T1& t1)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1);
    }

    template <class T0, class T1, class T2>
    void assign (T0& t0, T1& t1, T2& t2)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2);
    }

    template <class T0, class T1, class T2, class T3>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3);
    }

    template <class T0, class T1, class T2, class T3, class T4>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4, t5);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4, t5, t6);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4, t5, t6, t7);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4, t5, t6, t7, t8);
    }

    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void assign (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
    {
      holder_type::operator = (t0);
      tail_type::assign (t1, t2, t3, t4, t5, t6, t7, t8, t9);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение головы и хвоста cons-пары
///////////////////////////////////////////////////////////////////////////////////////////////////
    const_head_type  get_head () const { return holder_type::get (); }
    head_type        get_head ()       { return holder_type::get (); }
    const tail_type& get_tail () const { return *this; }
    tail_type&       get_tail ()       { return *this; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Head1, class Tail1> bool operator == (const cons<Head1, Tail1>& t) const
    {
      return get_head () == t.get_head () && get_tail () == t.get_tail ();
    }    
    
    template <class Head1, class Tail1> bool operator != (const cons<Head1, Tail1>& t) const
    {
      return get_head () != t.get_head () || get_tail () != t.get_tail ();
    }
    
    template <class Head1, class Tail1> bool operator < (const cons<Head1, Tail1>& t) const
    {
      return get_head () < t.get_head () || ( !(t.get_head () < get_head ()) && get_tail () < t.get_tail () );
    }
    
    template <class Head1, class Tail1> bool operator > (const cons<Head1, Tail1>& t) const
    {
      return get_head () > t.get_head () || ( !(t.get_head () > get_head ()) && get_tail () > t.get_tail () );
    }

    template <class Head1, class Tail1> bool operator <= (const cons<Head1, Tail1>& t) const
    {
      return get_head () <= t.get_head () || ( !(t.get_head () <= get_head ()) && get_tail () <= t.get_tail () );
    }
    
    template <class Head1, class Tail1> bool operator >= (const cons<Head1, Tail1>& t) const
    {
      return get_head () >= t.get_head () || ( !(t.get_head () >= get_head ()) && get_tail () >= t.get_tail () );
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализация последнего элемента cons-списка
///////////////////////////////////////////////////////////////////////////////////////////////////
template <> struct cons<mpl::null_type, mpl::null_type>
{
  enum { tuple_size = 0 };

  cons () {}

  void assign () {}

  bool operator == (const cons&) const { return true;  }

  template <class Head, class Tail> bool operator == (const cons<Head, Tail>&) const { return false; }
  template <class Head, class Tail> bool operator != (const cons<Head, Tail>&) const { return false; }
  template <class Head, class Tail> bool operator <  (const cons<Head, Tail>&) const { return false; }
  template <class Head, class Tail> bool operator >  (const cons<Head, Tail>&) const { return false; }
  template <class Head, class Tail> bool operator <= (const cons<Head, Tail>&) const { return false; }
  template <class Head, class Tail> bool operator >= (const cons<Head, Tail>&) const { return false; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Генератор кортежей
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct tuple_generator
{
  typedef cons<T0, typename tuple_generator<T1, T2, T3, T4, T5, T6, T7, T8, T9, mpl::null_type>::type> type;
};

template <> struct tuple_generator<mpl::null_type, mpl::null_type, mpl::null_type, mpl::null_type, mpl::null_type,
                                   mpl::null_type, mpl::null_type, mpl::null_type, mpl::null_type, mpl::null_type>
{
  typedef cons<mpl::null_type, mpl::null_type> type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Данная структура используется для выбора элемента кортежа по номеру
///////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t Index, class Head, class Tail>
struct tuple_element_selector<Index, cons<Head, Tail> >: public tuple_element_selector<Index-1, Tail> {};

template <class Head, class Tail>
struct tuple_element_selector<0, cons<Head, Tail> >
{
  typedef cons<Head, Tail>                             cons_type;
  typedef Head                                         head_type;
  typedef typename access_traits<Head>::const_type     const_head_type;
  typedef typename access_traits<Head>::non_const_type non_const_head_type;

  static const_head_type     get (const cons_type& c) { return c.get_head (); }
  static non_const_head_type get (cons_type& c)       { return c.get_head (); }
};

}

/*
    tuple
*/

/*
    Конструкторы
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (param0_type p0)
  : base (p0, mpl::null_type ())
  { }

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (param0_type p0, param1_type p1)
  : base (p0, p1)
  { }

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (param0_type p0, param1_type p1, param2_type p2)
  : base (p0, p1, p2)
  { }

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (param0_type p0, param1_type p1, param2_type p2, param3_type p3)
  : base (p0, p1, p2, p3)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
 (param0_type p0, 
  param1_type p1, 
  param2_type p2, 
  param3_type p3, 
  param4_type p4) 
    : base (p0, p1, p2, p3, p4)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
 (param0_type p0, 
  param1_type p1, 
  param2_type p2, 
  param3_type p3, 
  param4_type p4, 
  param5_type p5)
    : base (p0, p1, p2, p3, p4, p5)
  { }

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
 (param0_type p0, 
  param1_type p1, 
  param2_type p2, 
  param3_type p3, 
  param4_type p4, 
  param5_type p5, 
  param6_type p6)
    : base (p0, p1, p2, p3, p4, p5, p6)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
 (param0_type p0, 
  param1_type p1, 
  param2_type p2, 
  param3_type p3, 
  param4_type p4, 
  param5_type p5, 
  param6_type p6, 
  param7_type p7)
    : base (p0, p1, p2, p3, p4, p5, p6, p7)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
 (param0_type p0, 
  param1_type p1, 
  param2_type p2, 
  param3_type p3, 
  param4_type p4, 
  param5_type p5, 
  param6_type p6, 
  param7_type p7, 
  param8_type p8)
    : base (p0, p1, p2, p3, p4, p5, p6, p7, p8)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple
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
    : base (p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t)
  : base (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t))
  { }
  
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A1, class A2>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::tuple (const stl::pair<A1, A2>& p)
  : base (p.first, p.second)
  { }

/*
    Присваивание
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator =
 (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t)
{
  base::operator = (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));

  return *this;
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A1, class A2>
inline tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator =
 (const stl::pair<A1, A2>& t)
{
  base::assign (t.first, t.second);

  return *this;
}

/*
    Выборка
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> template <size_t number>
inline typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<number>::const_type
tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::get () const
{
  return element<number>::selector_type::get (*this);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> template <size_t number>
inline typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<number>::non_const_type
tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::get ()
{
  return element<number>::selector_type::get (*this);
}

template <size_t number, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<number>::const_type get
 (const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& t)
{
  return t.get<number> ();
}

template <size_t number, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<number>::non_const_type get
 (tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& t)
{
  return t.get<number> ();
}

/*
    Сравнение кортежей
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator == (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator == (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator != (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator != (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator < (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator < (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator > (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator > (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator <= (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator <= (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
template <class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline bool tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::operator >= (const tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>& t) const
{
  return base::operator >= (static_cast<const typename tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>::base&> (t));
}

/*
    Создание кортежей
*/

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица допустимых типов элементов кортежа
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct make_tuple_traits
{
  typedef T type;
};

template <class T, size_t size> struct make_tuple_traits<T [size]>
{
  typedef const T (&type)[size];
};

template <class T, size_t size> struct make_tuple_traits<const T [size]>
{
  typedef const T (&type)[size];
};

template <class T, size_t size> struct make_tuple_traits<volatile T [size]>
{
  typedef const volatile T (&type)[size];
};

template <class T, size_t size> struct make_tuple_traits<const volatile T [size]>
{
  typedef const volatile T (&type)[size];
};

template <class T> struct make_tuple_traits<T&>
{
  struct compile_time_error_do_not_use_with_reference_type; //compile-time error
  typedef compile_time_error_do_not_use_with_reference_type type;
};

template <class T> struct make_tuple_traits<reference_wrapper<T> >
{
  typedef T& type;
};

template <class T> struct make_tuple_traits<const reference_wrapper<T> >
{
  typedef T& type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа кортежа по типу его элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
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

}

inline tuple<> make_tuple ()
{
  return tuple<> ();
}

template <class T0>
inline typename detail::make_tuple_mapper<T0>::type make_tuple (const T0& t0)
{
  return typename detail::make_tuple_mapper<T0>::type (t0);
}

template <class T0, class T1>
inline typename detail::make_tuple_mapper<T0, T1>::type make_tuple (const T0& t0, const T1& t1)
{
  return typename detail::make_tuple_mapper<T0, T1>::type (t0, t1);
}

template <class T0, class T1, class T2>
inline typename detail::make_tuple_mapper<T0, T1, T2>::type make_tuple (const T0& t0, const T1& t1, const T2& t2)
{
  return typename detail::make_tuple_mapper<T0, T1, T2>::type (t0, t1, t2);
}

template <class T0, class T1, class T2, class T3>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3>::type make_tuple
 (const T0& t0, 
  const T1& t1, 
  const T2& t2, 
  const T3& t3)
{
  return typename detail::make_tuple_mapper<T0, T1, T2, T3>::type (t0, t1, t2, t3);
}

template <class T0, class T1, class T2, class T3, class T4>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4>::type make_tuple
 (const T0& t0, 
  const T1& t1, 
  const T2& t2, 
  const T3& t3, 
  const T4& t4)
{
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4>::type (t0, t1, t2, t3, t4);
}

template <class T0, class T1, class T2, class T3, class T4, class T5>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5>::type make_tuple
 (const T0& t0, 
  const T1& t1, 
  const T2& t2, 
  const T3& t3, 
  const T4& t4, 
  const T5& t5)
{
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5>::type (t0, t1, t2, t3, t4, t5);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6>::type make_tuple
 (const T0& t0, 
  const T1& t1, 
  const T2& t2, 
  const T3& t3, 
  const T4& t4, 
  const T5& t5, 
  const T6& t6)
{
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6>::type (t0, t1, t2, t3, t4, t5, t6);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7>::type make_tuple
 (const T0& t0, 
  const T1& t1, 
  const T2& t2, 
  const T3& t3, 
  const T4& t4, 
  const T5& t5, 
  const T6& t6, 
  const T7& t7)
{
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7>::type (t0, t1, t2, t3, t4, t5, t6, t7);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8>::type make_tuple
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
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8>::type (t0, t1, t2, t3, t4, t5, t6, t7, t8);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type make_tuple
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
  return typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type (t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
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

template <class T0, class T1>
inline tuple<T0&, T1&> tie (T0& t0, T1& t1)
{
  return tuple<T0&, T1&> (t0, t1);
}

template <class T0, class T1, class T2>
inline tuple<T0&, T1&, T2&> tie (T0& t0, T1& t1, T2& t2)
{
  return tuple<T0&, T1&, T2&> (t0, t1, t2);
}

template <class T0, class T1, class T2, class T3>
inline tuple<T0&, T1&, T2&, T3&> tie (T0& t0, T1& t1, T2& t2, T3& t3)
{
  return tuple<T0&, T1&, T2&, T3&> (t0, t1, t2, t3);
}

template <class T0, class T1, class T2, class T3, class T4>
inline tuple<T0&, T1&, T2&, T3&, T4&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4)
{
  return tuple<T0&, T1&, T2&, T3&, T4&> (t0, t1, t2, t3, t4);
}

template <class T0, class T1, class T2, class T3, class T4, class T5>
inline tuple<T0&, T1&, T2&, T3&, T4&, T5&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5)
{
  return tuple<T0&, T1&, T2&, T3&, T4&, T5&> (t0, t1, t2, t3, t4, t5);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6)
{
  return tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&> (t0, t1, t2, t3, t4, t5, t6);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7)
{
  return tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&> (t0, t1, t2, t3, t4, t5, t6, t7);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8)
{
  return tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&> (t0, t1, t2, t3, t4, t5, t6, t7, t8);
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&> tie (T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
{
  return tuple<T0&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&> (t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
}

/*
    Утилиты получения размера кортежа и получения типа его элемента
*/

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct tuple_size<tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  enum { value = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };
};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct tuple_size<const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  enum { value = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };
};

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4181)
#endif

template <size_t Index, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct tuple_element<Index, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<Index>::type type;
};

template <size_t Index, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct tuple_element<Index, const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef const typename tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::template element<Index>::type type;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif
