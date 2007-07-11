namespace detail
{

/*
    Класс для проверки: есть ли приведение из A* в B*
*/

template <class From, class To> struct light_is_convertible
{
  typedef char                       no_type;
  typedef struct { char dummy [2]; } yes_type;

  static no_type  test (...);
  static yes_type test (const To*);

  enum { value = sizeof (yes_type) == sizeof (test ((From*)0)) };
};

/*
    Селектор вызова функционального объекта
*/

template <int value> struct funcall_tag {};

typedef funcall_tag<0> default_funcall;     //f (arg1, ..., argN)
typedef funcall_tag<1> memfun_ptr_funcall;  //((*arg1).*f)(arg2, ..., argN)
typedef funcall_tag<2> memfun_ref_funcall;  //(arg1.*)(arg2, ..., argN)
typedef funcall_tag<3> memptr_ptr_funcall;  //(*arg1).*f
typedef funcall_tag<4> memptr_ref_funcall;  //arg1.*f

template <class Fn, class T=void, bool is_memfunc_ptr=is_member_function_pointer<Fn>::value> struct funcall_selector
{
  typedef typename functional_traits<Fn>::object_type object_type;
  typedef typename remove_reference_wrapper<T>::type argument_type;
  typedef typename mpl::select<light_is_convertible<argument_type, object_type>::value, memfun_ref_funcall, memfun_ptr_funcall>::type type;
};

template <class Fn, class T> struct funcall_selector<Fn, T, false>
{
  typedef default_funcall type;
};

template <class Ret, class T1, class T2> struct funcall_selector<Ret T1::*, T2, false>
{
  typedef typename remove_reference_wrapper<T2>::type argument_type;
  typedef typename mpl::select<light_is_convertible<argument_type, T1>::value, memptr_ref_funcall, memptr_ptr_funcall>::type type;
};

template <class Fn> struct funcall_selector<Fn, void, true>;

/*
    Снятие reference_wrapper
*/

template <class T>
inline T& unwrap (T& r)
{
  return r;
}

template <class T>
inline const T& unwrap (const T& r)
{
  return r;
}

template <class T> inline T& unwrap (const reference_wrapper<T>& r)
{
  return r.get ();
}

template <class T> inline const T& unwrap (const reference_wrapper<const T>& r)
{
  return r.get ();
}

template <class T> inline T& unwrap (reference_wrapper<T>& r)
{
  return r.get ();
}

template <class T> inline const T& unwrap (reference_wrapper<const T>& r)
{
  return r.get ();
}

/*
    Перегрузки вызовов функциональных объектов
*/

//перегрузка для функционального объекта без аргументов
template <class Fn, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, default_funcall, ResultTraits)
{
  return fn ();
}

//перегрузка для функционального объекта с 1-м аргументом
template <class Fn, class T1, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, default_funcall, ResultTraits)
{
  return fn (arg1);
}

//перегрузка для функционального объекта с 2-мя аргументами
template <class Fn, class T1, class T2, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, default_funcall, ResultTraits)
{
  return fn (arg1, arg2);
}

//перегрузка для функционального объекта с 3-мя аргументами
template <class Fn, class T1, class T2, class T3, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3);
}

//перегрузка для функционального объекта с 4-мя аргументами
template <class Fn, class T1, class T2, class T3, class T4, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4);
}

//перегрузка для функционального объекта с 5-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4, arg5);
}

//перегрузка для функционального объекта с 6-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4, arg5, arg6);
}

//перегрузка для функционального объекта с 7-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

//перегрузка для функционального объекта с 8-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

//перегрузка для функционального объекта с 9-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, default_funcall, ResultTraits)
{
  return fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

/*
    Перегрузки вызовов указателей на функцию-член класса
*/

//перегрузка для указателя на функцию-член класса с 1-м аргументом
template <class Fn, class T1, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)();
}

template <class Fn, class T1, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)();
}

//перегрузка для указателя на функцию-член класса с 2-мя аргументами
template <class Fn, class T1, class T2, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2);
}

template <class Fn, class T1, class T2, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2);
}

//перегрузка для указателя на функцию-член класса с 3-мя аргументами
template <class Fn, class T1, class T2, class T3, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3);
}

template <class Fn, class T1, class T2, class T3, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3);
}

//перегрузка для указателя на функцию-член класса с 4-мя аргументами
template <class Fn, class T1, class T2, class T3, class T4, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4);
}

template <class Fn, class T1, class T2, class T3, class T4, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4);
}

//перегрузка для указателя на функцию-член класса с 5-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4, arg5);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4, arg5);
}

//перегрузка для указателя на функцию-член класса с 6-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4, arg5, arg6);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6);
}

//перегрузка для указателя на функцию-член класса с 7-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7);
}

//перегрузка для указателя на функцию-член класса с 8-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

//перегрузка для указателя на функцию-член класса с 9-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, memfun_ref_funcall, ResultTraits)
{
  return (object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, memfun_ptr_funcall, ResultTraits)
{
  return ((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

/*
    Перегрузки вызовов указателей на объект-член класса
*/

//перегрузка для указателя на объект-член класса с 1-м аргументом
template <class MemPtr, class T1, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 2-мя аргументами
template <class MemPtr, class T1, class T2, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 3-мя аргументами
template <class MemPtr, class T1, class T2, class T3, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 4-мя аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 5-ю аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, T5&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 6-ю аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 7-ю аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 8-ю аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, T8&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, T8&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

//перегрузка для указателя на объект-член класса с 9-ю аргументами
template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, memptr_ref_funcall, ResultTraits)
{
  return object.*ptr;
}

template <class MemPtr, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class ResultTraits>
inline typename ResultTraits::type funcall_dispatch (const MemPtr& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, memptr_ptr_funcall, ResultTraits)
{
  return (*object_ptr).*ptr;
}

}

/*
    Диспетчеризация вызовов функциональных объектов
*/

//перегрузка для функционального объекта без аргументов
template <class Fn>
inline typename result_of<Fn()>::type funcall (Fn fn)
{
  return detail::funcall_dispatch (detail::unwrap (fn), typename detail::funcall_selector<Fn>::type (), result_of<Fn()> ());
}

//перегрузка для функционального объекта с 1-м аргументом
template <class Fn, class T1>
inline typename result_of<Fn(T1)>::type funcall (Fn fn, T1 arg1)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1)> ());
}

//перегрузка для функционального объекта с 2-мя аргументами
template <class Fn, class T1, class T2>
inline typename result_of<Fn(T1, T2)>::type funcall (Fn fn, T1 arg1, T2 arg2)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2)> ());
}

//перегрузка для функционального объекта с 3-мя аргументами
template <class Fn, class T1, class T2, class T3>
inline typename result_of<Fn(T1, T2, T3)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3)> ());
}

//перегрузка для функционального объекта с 4-мя аргументами
template <class Fn, class T1, class T2, class T3, class T4>
inline typename result_of<Fn(T1, T2, T3, T4)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4)> ());
}

//перегрузка для функционального объекта с 5-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5>
inline typename result_of<Fn(T1, T2, T3, T4, T5)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), detail::unwrap (arg5), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4, T5)> ());
}

//перегрузка для функционального объекта с 6-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline typename result_of<Fn(T1, T2, T3, T4, T5, T6)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), detail::unwrap (arg5), detail::unwrap (arg6), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4, T5, T6)> ());
}

//перегрузка для функционального объекта с 7-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline typename result_of<Fn(T1, T2, T3, T4, T5, T6, T7)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), detail::unwrap (arg5), detail::unwrap (arg6), detail::unwrap (arg7), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4, T5, T6, T7)> ());
}

//перегрузка для функционального объекта с 8-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline typename result_of<Fn(T1, T2, T3, T4, T5, T6, T7, T8)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), detail::unwrap (arg5), detail::unwrap (arg6), detail::unwrap (arg7), detail::unwrap (arg8), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4, T5, T6, T7, T8)> ());
}

//перегрузка для функционального объекта с 9-ю аргументами
template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename result_of<Fn(T1, T2, T3, T4, T5, T6, T7, T8, T9)>::type funcall (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return detail::funcall_dispatch (detail::unwrap (fn), detail::unwrap (arg1), detail::unwrap (arg2), detail::unwrap (arg3), detail::unwrap (arg4), detail::unwrap (arg5), detail::unwrap (arg6), detail::unwrap (arg7), detail::unwrap (arg8), detail::unwrap (arg9), typename detail::funcall_selector<Fn, T1>::type (), result_of<Fn(T1, T2, T3, T4, T5, T6, T7, T8, T9)> ());
}
