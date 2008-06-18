#ifndef __MYSTL_ITERATOR_TRAITS__
#define __MYSTL_ITERATOR_TRAITS__

#include <cstddef>

#include <stl/config.h>

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
template <class T> struct iterator_traits<T*>:                public iterator<random_access_iterator_tag, T> {};
template <class T> struct iterator_traits<const T*>:          public iterator<random_access_iterator_tag, T, ptrdiff_t, const T*, const T&> {};
template <class T> struct iterator_traits<volatile T*>:       public iterator<random_access_iterator_tag, T, ptrdiff_t, volatile T*, volatile T&> {};
template <class T> struct iterator_traits<const volatile T*>: public iterator<random_access_iterator_tag, T, ptrdiff_t, const volatile T*, const volatile T&> {};

template <> struct iterator_traits<bool>:           public iterator<int_iterator_tag, bool> {};
template <> struct iterator_traits<char>:           public iterator<int_iterator_tag, char> {};
template <> struct iterator_traits<signed char>:    public iterator<int_iterator_tag, signed char> {};
template <> struct iterator_traits<unsigned char>:  public iterator<int_iterator_tag, unsigned char> {};
template <> struct iterator_traits<short>:          public iterator<int_iterator_tag, short> {};
template <> struct iterator_traits<unsigned short>: public iterator<int_iterator_tag, unsigned short> {};
template <> struct iterator_traits<int>:            public iterator<int_iterator_tag, int> {};
template <> struct iterator_traits<unsigned int>:   public iterator<int_iterator_tag, unsigned int> {};
template <> struct iterator_traits<long>:           public iterator<int_iterator_tag, long> {};
template <> struct iterator_traits<unsigned long>:  public iterator<int_iterator_tag, unsigned long> {};
template <> struct iterator_traits<float>:          public iterator<float_iterator_tag, float> {};
template <> struct iterator_traits<double>:         public iterator<float_iterator_tag, double> {};
template <> struct iterator_traits<long double>:    public iterator<float_iterator_tag, long double> {};

#ifndef NO_WCHAR
template <> struct iterator_traits<wchar_t>: public iterator<int_iterator_tag, wchar_t> {};
#endif

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
