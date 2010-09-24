/*
    Проверка является ли T классом
*/

namespace detail
{

template <class T> struct is_class_helper
{
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
//      ~A () throw();  //деструктор закомментировани для обхода предупреждений об отсутствии виртуального деструктора (при компиляции gcc)

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

template <> struct is_abstract<void>:                public bool_constant<true> {};
template <> struct is_abstract<const void>:          public bool_constant<true> {};
template <> struct is_abstract<volatile void>:       public bool_constant<true> {};
template <> struct is_abstract<const volatile void>: public bool_constant<true> {};
