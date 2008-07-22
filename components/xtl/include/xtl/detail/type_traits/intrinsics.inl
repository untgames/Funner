/*
    Встроенные (реализуемые компилятором) проверки свойств типа
*/

namespace detail
{

namespace type_traits_intrinsics
{

#if defined (_MSC_VER) && (_MSC_VER >= 1400)

template <class T> struct is_union:                public bool_constant<__is_union (T)> {};
template <class T> struct is_pod:                  public bool_constant<__is_pod (T) && __has_trivial_constructor (T)> {};
template <class T> struct is_empty:                public bool_constant<__is_empty (T)> {};
template <class T> struct has_trivial_constructor: public bool_constant<__has_trivial_constructor (T)> {};
template <class T> struct has_trivial_copy:        public bool_constant<__has_trivial_copy (T)> {};
template <class T> struct has_trivial_assign:      public bool_constant<__has_trivial_assign (T)> {};
template <class T> struct has_trivial_destructor:  public bool_constant<__has_trivial_destructor (T)> {};
template <class T> struct has_nothrow_constructor: public bool_constant<__has_nothrow_constructor (T)> {};
template <class T> struct has_nothrow_copy:        public bool_constant<__has_nothrow_copy (T)> {};
template <class T> struct has_nothrow_assign:      public bool_constant<__has_nothrow_assign (T)> {};
template <class T> struct has_virtual_destructor:  public bool_constant<__has_virtual_destructor (T)> {};

#define XTL_HAS_TYPE_TRAITS_INTRINSICS

#else

template <class T> struct is_union:                public false_type {};
template <class T> struct is_pod:                  public false_type {};
template <class T> struct is_empty:                public false_type {};
template <class T> struct has_trivial_constructor: public false_type {};
template <class T> struct has_trivial_copy:        public false_type {};
template <class T> struct has_trivial_assign:      public false_type {};
template <class T> struct has_trivial_destructor:  public false_type {};
template <class T> struct has_nothrow_constructor: public false_type {};
template <class T> struct has_nothrow_copy:        public false_type {};
template <class T> struct has_nothrow_assign:      public false_type {};
template <class T> struct has_virtual_destructor:  public false_type {};

#endif

}

}
