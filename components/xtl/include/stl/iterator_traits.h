#ifndef __MYSTL_ITERATOR_TRAITS__
#define __MYSTL_ITERATOR_TRAITS__

#include <cstddef>

namespace stl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Категории итераторов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag: public input_iterator_tag {};
struct bidirectional_iterator_tag: public forward_iterator_tag {};
struct random_access_iterator_tag: public bidirectional_iterator_tag {};

//тэги "ложных" итераторов
struct int_iterator_tag {};
struct float_iterator_tag {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица конфигурации итераторов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iterator> struct iterator_traits 
{
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type        value_type;
  typedef typename Iterator::difference_type   difference_type;
  typedef typename Iterator::pointer           pointer;
  typedef typename Iterator::reference         reference;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Специализации конфигураций итераторов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Category>  struct iterator_traits_base
{
  typedef Category  iterator_category;
  typedef T         value_type;
  typedef ptrdiff_t difference_type;
  typedef T*        pointer;
  typedef T&        reference;
};

template <class T> struct iterator_traits<T*> 
{
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef T*                         pointer;
  typedef T&                         reference;
};

template <class T> struct iterator_traits<const T*> 
{
  typedef random_access_iterator_tag iterator_category;
  typedef T                          value_type;
  typedef ptrdiff_t                  difference_type;
  typedef const T*                   pointer;
  typedef const T&                   reference;
};

template <> struct iterator_traits<bool>:           public iterator_traits_base<bool, int_iterator_tag> {};
template <> struct iterator_traits<char>:           public iterator_traits_base<char, int_iterator_tag> {};
template <> struct iterator_traits<signed char>:    public iterator_traits_base<signed char, int_iterator_tag> {};
template <> struct iterator_traits<unsigned char>:  public iterator_traits_base<unsigned char, int_iterator_tag> {};
template <> struct iterator_traits<short>:          public iterator_traits_base<short, int_iterator_tag> {};
template <> struct iterator_traits<unsigned short>: public iterator_traits_base<unsigned short, int_iterator_tag> {};
template <> struct iterator_traits<int>:            public iterator_traits_base<int, int_iterator_tag> {};
template <> struct iterator_traits<unsigned int>:   public iterator_traits_base<unsigned int, int_iterator_tag> {};
template <> struct iterator_traits<long>:           public iterator_traits_base<long, int_iterator_tag> {};
template <> struct iterator_traits<unsigned long>:  public iterator_traits_base<unsigned long, int_iterator_tag> {};
template <> struct iterator_traits<float>:          public iterator_traits_base<float, float_iterator_tag> {};
template <> struct iterator_traits<double>:         public iterator_traits_base<double, float_iterator_tag> {};
template <> struct iterator_traits<long double>:    public iterator_traits_base<long double, float_iterator_tag> {};

#ifndef NO_WCHAR
template <> struct iterator_traits<wchar_t>: public iterator_traits_base<wchar_t, int_iterator_tag> {};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Category, class T, class Distance=ptrdiff_t, class Pointer=T*, class Reference=T&>
struct iterator 
{
  typedef Category  iterator_category;
  typedef T         value_type;
  typedef Distance  difference_type;
  typedef Pointer   pointer;
  typedef Reference reference;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Категория итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter> 
typename iterator_traits<Iter>::iterator_category iterator_category ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операции над итераторами
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Iter>
typename iterator_traits<Iter>::difference_type distance (Iter first, Iter last);

template <class Iter, class Distance> void distance (Iter first, Iter last, Distance& n);
template <class Iter, class Distance> void advance  (Iter&, Distance n); 

#include <stl/detail/iterator_traits.inl>

}

#endif
