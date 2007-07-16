namespace detail
{

//выбор типа аргумента
template <class T> struct invoker_argument     { typedef const T& type; };
template <class T> struct invoker_argument<T&> { typedef T&       type; };

//типы аргументов по умолчанию
struct invoker_default_arguments
{
  typedef struct void_argument_type {} arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type;
};

}

/*
    Специализации интерфейса обработчика полиморфного вызова функтора для разного числа аргументов
*/

//интерфейс обработчика полиморфного вызова функтора без аргументов
template <class Ret, class Base>
struct invoker<Ret (), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret result_type;

  virtual result_type operator () () = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 1-м аргументом
template <class T1, class Ret, class Base>
struct invoker<Ret (T1), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;

  virtual result_type operator () (arg1_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 2-мя аргументами
template <class T1, class T2, class Ret, class Base>
struct invoker<Ret (T1, T2), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;

  virtual result_type operator () (arg1_type, arg2_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 3-мя аргументами
template <class T1, class T2, class T3, class Ret, class Base>
struct invoker<Ret (T1, T2, T3), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 4-мя аргументами
template <class T1, class T2, class T3, class T4, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 5-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4, T5), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;
  typedef typename detail::invoker_argument<T5>::type arg5_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 6-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4, T5, T6), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;
  typedef typename detail::invoker_argument<T5>::type arg5_type;
  typedef typename detail::invoker_argument<T6>::type arg6_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 7-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4, T5, T6, T7), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;
  typedef typename detail::invoker_argument<T5>::type arg5_type;
  typedef typename detail::invoker_argument<T6>::type arg6_type;
  typedef typename detail::invoker_argument<T7>::type arg7_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 8-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4, T5, T6, T7, T8), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;
  typedef typename detail::invoker_argument<T5>::type arg5_type;
  typedef typename detail::invoker_argument<T6>::type arg6_type;
  typedef typename detail::invoker_argument<T7>::type arg7_type;
  typedef typename detail::invoker_argument<T8>::type arg8_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type) = 0;
};

//интерфейс обработчика полиморфного вызова функтора с 9-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class Ret, class Base>
struct invoker<Ret (T1, T2, T3, T4, T5, T6, T7, T8, T9), Base>: public detail::invoker_default_arguments, public Base
{
  typedef Ret                                         result_type;
  typedef typename detail::invoker_argument<T1>::type arg1_type;
  typedef typename detail::invoker_argument<T2>::type arg2_type;
  typedef typename detail::invoker_argument<T3>::type arg3_type;
  typedef typename detail::invoker_argument<T4>::type arg4_type;
  typedef typename detail::invoker_argument<T5>::type arg5_type;
  typedef typename detail::invoker_argument<T6>::type arg6_type;
  typedef typename detail::invoker_argument<T7>::type arg7_type;
  typedef typename detail::invoker_argument<T8>::type arg8_type;
  typedef typename detail::invoker_argument<T9>::type arg9_type;

  virtual result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) = 0;
};

/*
    Обработчик вызовов функтора
*/

/*
    Конструктор
*/

template <class Fn, class Base>
inline functional_invoker<Fn, Base>::functional_invoker (const Fn& in_fn)
  : fn (in_fn)
  { }

/*
    Взятие функтора
*/

template <class Fn, class Base>
inline Fn& functional_invoker<Fn, Base>::function ()
{
  return fn;
}

template <class Fn, class Base>
inline const Fn& functional_invoker<Fn, Base>::function () const
{
  return fn;
}

/*
    Вызов
*/
  
template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator () ()
{
  return funcall<result_type> (fn);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator () (arg1_type a1)
{
  return funcall<result_type> (fn, a1);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2)
{
  return funcall<result_type> (fn, a1, a2);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3)
{
  return funcall<result_type> (fn, a1, a2, a3);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4)
{
  return funcall<result_type> (fn, a1, a2, a3, a4);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5)
{
  return funcall<result_type> (fn, a1, a2, a3, a4, a5);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6)
{
  return funcall<result_type> (fn, a1, a2, a3, a4, a5, a6);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7)
{
  return funcall<result_type> (fn, a1, a2, a3, a4, a5, a6, a7);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7,
  arg8_type a8)
{
  return funcall<result_type> (fn, a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class Fn, class Base>
typename functional_invoker<Fn, Base>::result_type functional_invoker<Fn, Base>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7,
  arg8_type a8,
  arg9_type a9)
{
  return funcall<result_type> (fn, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
