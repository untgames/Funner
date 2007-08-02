namespace detail
{

/*
    Определение количества аргументов сигнатуры
*/

template <class Signature>
struct ArgumentsCount;

template <class Ret>
struct ArgumentsCount<Ret ()> { enum { value = 0 }; };

template <class Ret, class T1>
struct ArgumentsCount<Ret (T1)> { enum { value = 1 }; };

template <class Ret, class T1, class T2>
struct ArgumentsCount<Ret (T1, T2)> { enum { value = 2 }; };

template <class Ret, class T1, class T2, class T3>
struct ArgumentsCount<Ret (T1, T2, T3)> { enum { value = 3 }; };

template <class Ret, class T1, class T2, class T3, class T4>
struct ArgumentsCount<Ret (T1, T2, T3, T4)> { enum { value = 4 }; };

template <class Ret, class T1, class T2, class T3, class T4, class T5>
struct ArgumentsCount<Ret (T1, T2, T3, T4, T5)> { enum { value = 5 }; };

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
struct ArgumentsCount<Ret (T1, T2, T3, T4, T5, T6)> { enum { value = 6 }; };

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ArgumentsCount<Ret (T1, T2, T3, T4, T5, T6, T7)> { enum { value = 7 }; };

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ArgumentsCount<Ret (T1, T2, T3, T4, T5, T6, T7, T8)> { enum { value = 8 }; };

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ArgumentsCount<Ret (T1, T2, T3, T4, T5, T6, T7, T8, T9)> { enum { value = 9 }; };

/*
    Помещение в стек элемента с учётом игнорирования аргументов
*/

struct Ignore {};

template <class T> inline void push (Stack& stack, const T& value)
{
  stack.Push (value);
}

inline void push (Stack& stack, Ignore)
{
}

}

/*
   Регистрация функтора в окружении Lua
*/

template <class Signature, class Fn>
inline void Environment::RegisterFunction (const char* name, Fn fn)
{
  RegisterFunctionCore (name, new detail::InvokerImpl<Signature, Fn> (fn), detail::ArgumentsCount<Signature>::value);
}

/*
   Вызов функций Lua
*/

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret Environment::InvokeDispatch
 (const char* name,
  size_t      args_count,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  const T7&   arg7,
  const T8&   arg8,
  const T9&   arg9,
  Result<Ret>) const
{
  detail::Stack stack = Stack ();

  stack.PushFunction (name);
  detail::push       (stack, arg1);
  detail::push       (stack, arg2);
  detail::push       (stack, arg3);
  detail::push       (stack, arg4);
  detail::push       (stack, arg5);
  detail::push       (stack, arg6);
  detail::push       (stack, arg7);
  detail::push       (stack, arg8);
  detail::push       (stack, arg9);
  InvokeCore         (args_count, 1);

  Ret ret_value = stack.Get<Ret> (-1);

  stack.Pop (1);

  return ret_value;  
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void Environment::InvokeDispatch
 (const char* name,
  size_t      args_count,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  const T7&   arg7,
  const T8&   arg8,
  const T9&   arg9,
  Result<void>) const
{
  detail::Stack stack = Stack ();

  stack.PushFunction (name);
  detail::push       (stack, arg1);
  detail::push       (stack, arg2);
  detail::push       (stack, arg3);
  detail::push       (stack, arg4);
  detail::push       (stack, arg5);
  detail::push       (stack, arg6);
  detail::push       (stack, arg7);
  detail::push       (stack, arg8);
  detail::push       (stack, arg9);
  InvokeCore         (args_count, 0);
}

template <class Ret>
inline Ret Environment::Invoke (const char* name, Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 0, detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1>
inline Ret Environment::Invoke (const char* name, const T1& arg1, Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 1, arg1, detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2>
inline Ret Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 2, arg1, arg2, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3>
inline Ret Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, const T3& arg3, Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 3, arg1, arg2, arg3, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4>
inline Ret Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 4, arg1, arg2, arg3, arg4, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4, class T5>
inline Ret Environment::Invoke
 (const char* name,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 5, arg1, arg2, arg3, arg4, arg5, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                              detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret Environment::Invoke
 (const char* name,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 6, arg1, arg2, arg3, arg4, arg5, arg6, detail::Ignore (),
                              detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret Environment::Invoke
 (const char* name,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  const T7&   arg7,
  Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 7, arg1, arg2, arg3, arg4, arg5, arg6, arg7, detail::Ignore (), detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret Environment::Invoke
 (const char* name,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  const T7&   arg7,
  const T8&   arg8,
  Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 8, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, detail::Ignore (), result);
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret Environment::Invoke
 (const char* name,
  const T1&   arg1,
  const T2&   arg2,
  const T3&   arg3,
  const T4&   arg4,
  const T5&   arg5,
  const T6&   arg6,
  const T7&   arg7,
  const T8&   arg8,
  const T9&   arg9,
  Result<Ret> result) const
{
  return InvokeDispatch<Ret> (name, 9, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, result);
}

inline void Environment::Invoke (const char* name, Result<void> result) const
{
  InvokeDispatch (name, 0, detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class T1>
inline void Environment::Invoke (const char* name, const T1& arg1, Result<void> result) const
{
  InvokeDispatch (name, 1, arg1, detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2>
inline void Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, Result<void> result) const
{
  InvokeDispatch (name, 2, arg1, arg2, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2, class T3>
inline void Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, const T3& arg3, Result<void> result) const
{
  InvokeDispatch (name, 3, arg1, arg2, arg3, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4>
inline void Environment::Invoke (const char* name, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, Result<void> result) const
{
  InvokeDispatch (name, 4, arg1, arg2, arg3, arg4, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4, class T5>
inline void Environment::Invoke
 (const char*  name,
  const T1&    arg1,
  const T2&    arg2,
  const T3&    arg3,
  const T4&    arg4,
  const T5&    arg5,
  Result<void> result) const
{
  InvokeDispatch (name, 5, arg1, arg2, arg3, arg4, arg5, detail::Ignore (), detail::Ignore (), detail::Ignore (),
                  detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
inline void Environment::Invoke
 (const char*  name,
  const T1&    arg1,
  const T2&    arg2,
  const T3&    arg3,
  const T4&    arg4,
  const T5&    arg5,
  const T6&    arg6,
  Result<void> result) const
{
  InvokeDispatch (name, 6, arg1, arg2, arg3, arg4, arg5, arg6, detail::Ignore (), detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void Environment::Invoke
 (const char*  name,
  const T1&    arg1,
  const T2&    arg2,
  const T3&    arg3,
  const T4&    arg4,
  const T5&    arg5,
  const T6&    arg6,
  const T7&    arg7,
  Result<void> result) const
{
  InvokeDispatch (name, 7, arg1, arg2, arg3, arg4, arg5, arg6, arg7, detail::Ignore (), detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void Environment::Invoke
 (const char*  name,
  const T1&    arg1,
  const T2&    arg2,
  const T3&    arg3,
  const T4&    arg4,
  const T5&    arg5,
  const T6&    arg6,
  const T7&    arg7,
  const T8&    arg8,
  Result<void> result) const
{
  InvokeDispatch (name, 8, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, detail::Ignore (), result);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void Environment::Invoke
 (const char*  name,
  const T1&    arg1,
  const T2&    arg2,
  const T3&    arg3,
  const T4&    arg4,
  const T5&    arg5,
  const T6&    arg6,
  const T7&    arg7,
  const T8&    arg8,
  const T9&    arg9,
  Result<void> result) const  
{
  InvokeDispatch (name, 9, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, result);
}
