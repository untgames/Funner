/*
    Применение функтора к списку аргументов
*/

namespace detail
{

//селектор количества аргументов вызова
template <size_t count> struct apply_selector {};

//обёртка аргументов, возвращаемых по значению
template <class T> T& wrap_argument (T& x)
{
  return x;
}

template <class T> const T& wrap_argument (const T& x)
{
  return x;
}

//перегрузка для функционального объекта без аргументов
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args&, Evalutor&, apply_selector<0>)
{
  return funcall<Ret> (fn);
}

//перегрузка для функционального объекта c 1-м аргументом
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<1>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)));
}

//перегрузка для функционального объекта c 2-мя аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<2>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)));
}

//перегрузка для функционального объекта c 3-мя аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<3>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)));
}

//перегрузка для функционального объекта c 4-мя аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<4>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)));
}

//перегрузка для функционального объекта c 5-ю аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<5>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)),
                           wrap_argument (e.eval<4> (args)));
}

//перегрузка для функционального объекта c 6-ю аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<6>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)),
                           wrap_argument (e.eval<4> (args)), wrap_argument (e.eval<5> (args)));
}

//перегрузка для функционального объекта c 7-ю аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<7>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)),
                           wrap_argument (e.eval<4> (args)), wrap_argument (e.eval<5> (args)), wrap_argument (e.eval<6> (args)));
}

//перегрузка для функционального объекта c 8-ю аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<8>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)),
                           wrap_argument (e.eval<4> (args)), wrap_argument (e.eval<5> (args)), wrap_argument (e.eval<6> (args)), wrap_argument (e.eval<7> (args)));
}

//перегрузка для функционального объекта c 9-ю аргументами
template <class Ret, class Fn, class Args, class Evalutor>
inline Ret apply_dispatch (Fn& fn, Args& args, Evalutor& e, apply_selector<9>)
{
  return funcall<Ret> (fn, wrap_argument (e.eval<0> (args)), wrap_argument (e.eval<1> (args)), wrap_argument (e.eval<2> (args)), wrap_argument (e.eval<3> (args)),
                           wrap_argument (e.eval<4> (args)), wrap_argument (e.eval<5> (args)), wrap_argument (e.eval<6> (args)), wrap_argument (e.eval<7> (args)),
                           wrap_argument (e.eval<8> (args)));
}

//вычислитель аргументов кортежа
struct tuple_argument_evalutor
{
  template <size_t I, class Tuple> static typename tuple_element<I, Tuple>::type eval (Tuple& args)
  {
    return get<I> (args);
  }
};

}

template <class Ret, size_t ArgumentsCount, class Fn, class Args, class Evalutor>
inline Ret apply (Fn& fn, Args& args, Evalutor& evalutor)
{
  return detail::apply_dispatch<Ret> (unwrap (fn), args, evalutor, detail::apply_selector<ArgumentsCount> ());
}

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret apply (Fn& fn, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args)
{
  enum { arguments_count = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };

  return apply<Ret, arguments_count> (fn, args, detail::tuple_argument_evalutor ());
}

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret apply (Fn& fn, const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args)
{
  enum { arguments_count = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };

  return apply<Ret, arguments_count> (fn, args, detail::tuple_argument_evalutor ());
}
