/*
    Определение типа возвращаемого значения функционального объекта:
      1) если тип является указателем на функцию или указателем на функцию-член,
         то тип результата определяется из сигнатуры: R f(Arg1, Arg2, ..., ArgN) -> R;
      2) если тип является сигнатурой F(Arg1, ..., ArgN), то:
         2.1) если F - указатель на функцию или указатель на функцию-член, то тип возвращаемого 
              значения определяется из сигнатуры F;
         2.2) иначе (если F - пользовательский тип данных):
              2.2.1) если количество аргументов == 0 -> тип возвращаемого значения void;
              2.2.2) если F содержит тип result_type, то тип возвращаемого значения result_type
              2.2.3) иначе тип возвращаемого значения result<F(Arg1, ..., ArgN)>::type              
*/

namespace detail
{

//определение наличия типа Fn::result_type
struct no_result_type_tag {};

template <class T, class result_type = no_result_type_tag> struct has_result_type: public T
{
  typedef char                       no_type;
  typedef struct { char dummy [2]; } yes_type;

  static yes_type test (...);
  static no_type  test (void (*)(no_result_type_tag));
  
  enum { value = sizeof (test (static_cast<void (*)(result_type)> (0))) == sizeof (yes_type) };
};

//определение типа возвращаемого значения пользовательского типа Fn
template <class Fn, class FnArgs, bool HasResultType=has_result_type<Fn>::value> struct user_fn_result_of
{
  typedef typename Fn::result_type type;
};

template <class Fn, class FnArgs> struct user_fn_result_of<Fn, FnArgs, false>
{
  typedef typename Fn::template result<FnArgs>::type type;
};

template <class Fn> struct user_fn_result_of<Fn, Fn(void), false>
{
  typedef void type;
};

//определение типа возвращаемого значения сигнатуры Fn
template <class Fn, class FnArgs, class Traits=functional_traits<Fn>,
          bool IsMemFunOrPtrFun=Traits::is_ptrfun || Traits::is_memfun> struct signature_result_of
{
  typedef typename Traits::result_type type;
};

template <class Fn, class FnArgs, class Traits> struct signature_result_of<Fn, FnArgs, Traits, false>
{
  typedef typename user_fn_result_of<Fn, FnArgs>::type type;
};

template <class T, class Ret, class FnArgs, class Traits> struct signature_result_of<Ret T::*, FnArgs, Traits, false>
{
  typedef Ret& type;
};

//определение типа возвращаемого значения если Fn не сигнатура
template <class Fn, class Traits, bool IsMemFunOrPtrFun = Traits::is_memfun || Traits::is_ptrfun> 
struct non_signature_result_of
{
  typedef typename Traits::result_type type;
};

template <class Fn, class Traits> struct non_signature_result_of<Fn, Traits, false>
{
  typedef typename Fn::result_type type;
};

template <class T, class Ret, class Traits> struct non_signature_result_of<Ret T::*, Traits, false>
{
  typedef Ret& type;
};

//комбинация указанных выше случаев
template <class Fn, class Traits = functional_traits<Fn>, bool IsSignature = Traits::is_function> struct result_of
{
  typedef typename signature_result_of<typename Traits::result_type, Fn>::type type;
};

template <class Fn, class Traits> struct result_of<Fn, Traits, false>
{
  typedef typename non_signature_result_of<Fn, Traits>::type type;
};

}

template <class Fn> struct result_of: public detail::result_of<Fn> {};

/*
    Обёртка над ссылкой
*/

/*
    Конструктор
*/

template <class T>
inline reference_wrapper<T>::reference_wrapper (T& ref)
  : ptr (&ref)
  { }

template <class T>
inline reference_wrapper<T>::operator T& () const
{
  return *ptr;
}

template <class T>
inline T& reference_wrapper<T>::get () const
{
  return *ptr;
}

/*
    Создание обёрток над ссылками
*/

template <class T>
inline reference_wrapper<T> ref (T& r)
{
  return reference_wrapper<T> (r);
}

template <class T>
inline reference_wrapper<const T> cref (const T& r)
{
  return reference_wrapper<const T> (r);
}

template <class T>
inline reference_wrapper<T> ref (reference_wrapper<T> rw)
{
  return rw;
}

template <class T>
inline reference_wrapper<const T> cref (reference_wrapper<T> rw)
{
  return rw;
}

/*
    Очистка reference_wrapper
*/

template <class T> struct remove_reference_wrapper                                       { typedef T type; };
template <class T> struct remove_reference_wrapper<reference_wrapper<T> >                { typedef T type; };
template <class T> struct remove_reference_wrapper<const reference_wrapper<T> >          { typedef T type; };
template <class T> struct remove_reference_wrapper<volatile reference_wrapper<T> >       { typedef T type; };
template <class T> struct remove_reference_wrapper<const volatile reference_wrapper<T> > { typedef T type; };

template <class T> inline T& unwrap (T& r)
{
  return r;
}

template <class T> inline T& unwrap (const reference_wrapper<T>& r)
{
  return r.get ();
}

template <class T> inline T& unwrap (reference_wrapper<T>& r)
{
  return r.get ();
}

/*
    Проверка равенства функционалов
*/

template <class Fn1, class Fn2>
inline bool function_equal (const Fn1& f1, const Fn2& f2)
{
  return f1 == f2;
}

template <class Fn1, class Fn2>
inline bool function_equal (const reference_wrapper<Fn1>& f1, const Fn2& f2)
{
  return function_equal (unwrap (f1), f2);
}

template <class Fn1, class Fn2>
inline bool function_equal (const Fn1& f1, const reference_wrapper<Fn2>& f2)
{
  return function_equal (f1, unwrap (f2));
}

template <class Fn1, class Fn2>
inline bool function_equal (const reference_wrapper<Fn1>& f1, const reference_wrapper<Fn2>& f2)
{
  return function_equal (unwrap (f1), unwrap (f2));
}
