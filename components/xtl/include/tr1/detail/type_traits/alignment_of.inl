#ifndef MYTR1_TYPE_TRAITS_ALIGNMENT_OF_HEADER
#define MYTR1_TYPE_TRAITS_ALIGNMENT_OF_HEADER

/*
    Определение выравнивания
*/

namespace detail
{

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4121) // alignment is sensitive to packing
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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

}

template <class T> struct alignment_of:                      public integral_constant<size_t, detail::alignment_of_helper<T>::value> {};
template <class T> struct alignment_of<T&>:                  public alignment_of<T*> {};
template <>        struct alignment_of<void>:                public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const void>:          public integral_constant<size_t, 0> {};
template <>        struct alignment_of<volatile void>:       public integral_constant<size_t, 0> {};
template <>        struct alignment_of<const volatile void>: public integral_constant<size_t, 0> {};

#endif
