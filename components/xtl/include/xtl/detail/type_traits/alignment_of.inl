#ifndef XTL_TYPE_TRAITS_ALIGNMENT_OF_HEADER
#define XTL_TYPE_TRAITS_ALIGNMENT_OF_HEADER

/*
    Определение выравнивания
*/

namespace detail
{

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4121) // alignment is sensitive to packing
  #pragma warning(disable: 4624) //'xtl::type_traits::detail::alignment_of_helper<T>::storage' : destructor could not be generated because a base class destructor is inaccessible
#endif

//вспомогательный класс определения выравнивания типа
template <class T> struct alignment_of_helper
{
  struct storage
  {
    T    t;
    char c;  
    
    storage ();
  };
  
  enum {
    align = sizeof (storage) - sizeof (T),
    value = align < sizeof (T) ? align : sizeof (T)
  };
};

template <class T, bool is_abstract_class=is_abstract<T>::value> struct alignment_of_abstract_helper
{
  enum {
    align = alignment_of_helper<T>::align,
    value = alignment_of_helper<T>::value
  };
};

template <class T> struct alignment_of_abstract_helper<T, true>
{
  enum {
    align = 0,
    value = 0
  };
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

}

template <class T> struct alignment_of:                      public integral_constant<size_t, detail::alignment_of_abstract_helper<T>::value> {};
template <class T> struct alignment_of<T&>:                  public alignment_of<T*> {};
template <>        struct alignment_of<void>:                public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const void>:          public integral_constant<size_t, 0> {};
template <>        struct alignment_of<volatile void>:       public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const volatile void>: public integral_constant<size_t, 0> {};

#endif
