//размер кортежа
template <class T1, class T2> struct tuple_size<stl::pair<T1, T2> > : public int_constant<2> {};

//выбор типа элемента кортежа
template <class T1, class T2> struct tuple_element<0, stl::pair<T1, T2> > { typedef typename T1 type; };
template <class T1, class T2> struct tuple_element<1, stl::pair<T1, T2> > { typedef typename T2 type; };

namespace detail
{

template <size_t I> struct pair_element;

template <> struct pair_element<0>
{
  template <class T1, class T2> static T1&       get (stl::pair<T1, T2>& p)       { return p.first; }
  template <class T1, class T2> static const T1& get (const stl::pair<T1, T2>& p) { return p.first; }
};

template <> struct pair_element<1>
{
  template <class T1, class T2> static T2&       get (stl::pair<T1, T2>& p)       { return p.second; }
  template <class T1, class T2> static const T2& get (const stl::pair<T1, T2>& p) { return p.second; }
};

}

template <size_t I, class T1, class T2>
inline typename tuple_element<I, stl::pair<T1, T2> >::type& get (stl::pair<T1, T2>& p)
{
  return detail::pair_element<I>::get (p);  
}

template <size_t I, class T1, class T2>
inline const typename tuple_element<I, stl::pair<T1, T2> >::type& get (const stl::pair<T1, T2>&)
{
  return detail::pair_element<I>::get (p);
}
