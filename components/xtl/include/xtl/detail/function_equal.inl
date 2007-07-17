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
