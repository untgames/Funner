/*
    Проверка является ли T классом
*/

namespace detail
{

template <class T> struct is_class_helper
{
  typedef char no_type;
  struct  yes_type { char dummy [2]; };

  template <class U> static yes_type is_class_tester (void (U::*)());
  template <class U> static no_type  is_class_tester (...);
  
  enum { value = (sizeof (is_class_tester<T> (0)) == sizeof (yes_type)) && !is_union<T>::value };
};

}

template <class T> struct is_class: public bool_constant<detail::is_class_helper<typename remove_cv<T>::type>::value> {};

/*
    Проверка является ли T перечислением
*/

namespace detail
{

struct int_convertible { int_convertible (int); };

template <bool not_enum = true> struct enum_helper
{
  template <class T> struct type: public false_type {};
};

template <> struct enum_helper<false>
{
  template <class T> struct type: public is_convertible<typename add_reference<T>::type, int_convertible> {};
};

}

template <class T> struct is_enum:
         public detail::enum_helper<is_arithmetic<T>::value || is_reference<T>::value || is_function<T>::value ||
                                    is_class<T>::value || is_union<T>::value || is_array<T>::value>::template type<T> {};

/*
    Проверка является ли T полиморфным типом
*/

namespace detail
{

template <class T, bool is_a_class=is_class<T>::value> struct is_polymorphic_helper
{
  typedef typename remove_cv<T>::type base;
  
  struct A: public base
  {
      A  ();
      ~A () throw();

      char padding [256];
      
   private:
      A (const A&);
      A& operator = (const A&);  
  };
  
  struct B: public base
  {
      B  ();
      virtual ~B () throw();

      char padding [256];

   private:
      B (const B&);
      B& operator = (const B&);  
  };  
  
  enum { value = sizeof (A) == sizeof (B) };
};

template <class T> struct is_polymorphic_helper<T, false>: public false_type {};

}

template <class T> struct is_polymorphic: public bool_constant<detail::is_polymorphic_helper<T>::value> {};
                                    
/*
    Проверка является ли класс абстрактным
*/

namespace detail
{

template <class T, bool is_a_complete_class=is_class<T>::value && sizeof (T)> struct is_abstract_helper
{
  typedef char no_type;
  struct  yes_type { char dummy [2]; };

  template <class U> static no_type  check_sig (U (*)[1]);
  template <class U> static yes_type check_sig (...);
   
  enum {
    sig_size = sizeof (check_sig<T> (0)),
    value    = sig_size == sizeof (yes_type)
  };
};

template <class T> struct is_abstract_helper<T, false> { enum { value = false }; };

}

template <class T> struct is_abstract: public bool_constant<detail::is_abstract_helper<T>::value> {};

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
