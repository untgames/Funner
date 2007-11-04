/*
    Создание аргумента функции-шлюза
*/

namespace detail
{ 

template <class T>
inline xtl::any make_invoker_argument (T& value)
{ 
  return make_ref_any (value);
}

inline char               make_invoker_argument (char value)               { return value; }
inline signed char        make_invoker_argument (signed char value)        { return value; }
inline unsigned char      make_invoker_argument (unsigned char value)      { return value; }
inline short              make_invoker_argument (short value)              { return value; }
inline unsigned short     make_invoker_argument (unsigned short value)     { return value; }
inline int                make_invoker_argument (int value)                { return value; }
inline unsigned int       make_invoker_argument (unsigned int value)       { return value; }
inline long               make_invoker_argument (long value)               { return value; }
inline unsigned long      make_invoker_argument (unsigned long value)      { return value; }
inline float              make_invoker_argument (float value)              { return value; }
inline const double&      make_invoker_argument (const double& value)      { return value; }
inline const long double& make_invoker_argument (const long double& value) { return value; }
inline const char*        make_invoker_argument (const char* string)       { return string; }
inline const char*        make_invoker_argument (char* string)             { return string; }

/*
    Извлечение аргументов из стека
*/

//общая версия
template <class T> struct argument_selector
{
  static T get (IStack& stack, size_t index)
  {
    return xtl::any_multicast<T> (stack.GetVariant (index));
  }
};

//извлечение целочисленного аргумента
template <class T> struct int_argument_selector
{
  static T get (IStack& stack, size_t index) { return static_cast<T> (stack.GetInteger (index)); }
};

//извлечение вещественного аргумента
template <class T> struct float_argument_selector
{
  static T get (IStack& stack, size_t index) { return static_cast<T> (stack.GetFloat (index)); }
};

//извлечение void-указателей
struct raw_pointer_argument_selector
{
  static void* get (IStack& stack, size_t index) { return stack.GetPointer (index); }
};

//извлечение строк
struct string_argument_selector
{
  static const char* get (IStack& stack, size_t index) { return stack.GetString (index); }
};

//специализации взятия аргумента для различных типов данных
template <> struct argument_selector<char>:                 public int_argument_selector<char> {};
template <> struct argument_selector<signed char>:          public int_argument_selector<signed char> {};
template <> struct argument_selector<unsigned char>:        public int_argument_selector<unsigned char> {};
template <> struct argument_selector<short>:                public int_argument_selector<short> {};
template <> struct argument_selector<unsigned short>:       public int_argument_selector<unsigned short> {};
template <> struct argument_selector<int>:                  public int_argument_selector<int> {};
template <> struct argument_selector<unsigned int>:         public int_argument_selector<unsigned int> {};
template <> struct argument_selector<long>:                 public int_argument_selector<long> {};
template <> struct argument_selector<unsigned long>:        public int_argument_selector<unsigned long> {};
template <> struct argument_selector<float>:                public float_argument_selector<float> {};
template <> struct argument_selector<double>:               public float_argument_selector<double> {};
template <> struct argument_selector<long double>:          public float_argument_selector<long double> {};
template <> struct argument_selector<void*>:                public raw_pointer_argument_selector {};
template <> struct argument_selector<const void*>:          public raw_pointer_argument_selector {};
template <> struct argument_selector<const volatile void*>: public raw_pointer_argument_selector {};
template <> struct argument_selector<const char*>:          public string_argument_selector {};
template <> struct argument_selector<const volatile char*>: public string_argument_selector {};

template <class Traits, class Allocator>
struct argument_selector<stl::basic_string<char, Traits, Allocator> >: public string_argument_selector {};

//функция взятия аргумента из стека
template <class T>
inline T get_argument (IStack& stack, size_t index)
{
  return argument_selector<T>::get (stack, index);
}

/*
    Помещение аргумента в стек
*/

struct ignore {};

template <class T> inline void push_argument (IStack& stack, const T& value)
{
  stack.Push (make_invoker_argument (value));
}

inline void push_argument (IStack& stack, ignore)
{
}

/*
    Селектор аргументов
*/

template <class FunctionalTraits, size_t I, bool is_memfun=FunctionalTraits::is_memfun>
struct functional_argument_traits
{
  typedef typename FunctionalTraits::template argument<I+1>::type argument_type;
  
  enum { argument_index = I };
};

template <class FunctionalTraits, size_t I>
struct functional_argument_traits<FunctionalTraits, I, true>
{
  typedef typename FunctionalTraits::template argument<I>::type argument_type;
  
  enum { argument_index = I };
};

template <class FunctionalTraits>
struct functional_argument_traits<FunctionalTraits, 0, true>
{
  typedef typename FunctionalTraits::object_type& argument_type;
  
  enum { argument_index = 0 };
};

template <class FunctionalTraits>
struct stack_argument_selector
{
  template <size_t I> struct traits: public functional_argument_traits<FunctionalTraits, I> {};  

  template <size_t I> static typename traits<I>::argument_type eval (IStack& stack)
  {
    return get_argument<typename traits<I>::argument_type> (stack, traits<I>::argument_index);
  }
};

/*
    Функтор шлюза
*/

template <class FnTraits, class Fn, class Ret=typename FnTraits::result_type>
struct invoker_impl
{
  enum {
    arguments_count = FnTraits::arguments_count + FnTraits::is_memfun,
    results_count   = 1
  };

  invoker_impl (const Fn& in_fn) : fn (in_fn) {}    

  void operator () (IStack& stack) const
  {
    push_argument (stack, xtl::apply<typename FnTraits::result_type, arguments_count> (fn, stack, stack_argument_selector<FnTraits> ()));
  }

  Fn fn;
};

template <class FnTraits, class Fn>
struct invoker_impl<FnTraits, Fn, void>
{
  enum {
    arguments_count = FnTraits::arguments_count + FnTraits::is_memfun,
    results_count   = 0
  };

  invoker_impl (const Fn& in_fn) : fn (in_fn) {}

  void operator () (IStack& stack) const
  {
    xtl::apply<void, arguments_count> (fn, stack, stack_argument_selector<FnTraits> ());
  }

  Fn fn;
};

/*
    Функция-диспетчер вызова шлюза
*/

template <size_t ArgsCount, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline Ret invoke_dispatch
 (IInterpreter& interpreter,
  const char*   name,
  const T1&     arg1,
  const T2&     arg2,
  const T3&     arg3,
  const T4&     arg4,
  const T5&     arg5,
  const T6&     arg6,
  const T7&     arg7,
  const T8&     arg8,
  const T9&     arg9,
  const T10&    arg10,
  xtl::type<Ret>)
{
  IStack& stack = interpreter.Stack ();

  stack.PushSymbol (name);
  push_argument    (stack, arg1);
  push_argument    (stack, arg2);
  push_argument    (stack, arg3);
  push_argument    (stack, arg4);
  push_argument    (stack, arg5);
  push_argument    (stack, arg6);
  push_argument    (stack, arg7);
  push_argument    (stack, arg8);
  push_argument    (stack, arg9);
  push_argument    (stack, arg10);

  interpreter.Invoke (ArgsCount, 1);

  Ret result = get_argument<Ret> (stack, stack.Size () - 1);

  stack.Pop (1);

  return result;
}

template <size_t ArgsCount, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void invoke_dispatch
 (IInterpreter& interpreter,
  const char*   name,
  const T1&     arg1,
  const T2&     arg2,
  const T3&     arg3,
  const T4&     arg4,
  const T5&     arg5,
  const T6&     arg6,
  const T7&     arg7,
  const T8&     arg8,
  const T9&     arg9,
  const T10&    arg10,
  xtl::type<void>)
{
  IStack& stack = interpreter.Stack ();

  stack.PushSymbol (name);
  push_argument    (stack, arg1);
  push_argument    (stack, arg2);
  push_argument    (stack, arg3);
  push_argument    (stack, arg4);
  push_argument    (stack, arg5);
  push_argument    (stack, arg6);
  push_argument    (stack, arg7);
  push_argument    (stack, arg8);
  push_argument    (stack, arg9);
  push_argument    (stack, arg10);

  interpreter.Invoke (ArgsCount, 0);
}

}

/*
    Создание шлюза для произвольного функтора с известной сигнатурой
*/

template <class Signature, class Fn>
inline Invoker make_invoker (Fn fn)
{
  typedef detail::invoker_impl<xtl::functional_traits<Signature>, Fn> invoker_type;

  return Invoker (invoker_type (fn), invoker_type::arguments_count, invoker_type::results_count);
}

template <class Fn>
inline Invoker make_invoker (Fn fn)
{
  return make_invoker<Fn, Fn> (fn);
}

/*
    Вызов шлюза
*/

template <class Ret>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name)
{
  using namespace detail;

  return invoke_dispatch<0> (interpreter, fn_name, ignore (), ignore (), ignore (), ignore (), ignore (),
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1)
{
  using namespace detail;

  return invoke_dispatch<1> (interpreter, fn_name, arg1, ignore (), ignore (), ignore (), ignore (),
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2)
{
  using namespace detail;

  return invoke_dispatch<2> (interpreter, fn_name, arg1, arg2, ignore (), ignore (), ignore (),
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2, const T3& arg3)
{
  using namespace detail;

  return invoke_dispatch<3> (interpreter, fn_name, arg1, arg2, arg3, ignore (), ignore (),
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4)
{
  using namespace detail;

  return invoke_dispatch<4> (interpreter, fn_name, arg1, arg2, arg3, arg4, ignore (),
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5)
{
  using namespace detail;

  return invoke_dispatch<5> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5,
                             ignore (), ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6)
{
  using namespace detail;

  return invoke_dispatch<6> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6,
                             ignore (), ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7)
{
  using namespace detail;

  return invoke_dispatch<7> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7,
                             ignore (), ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8)
{
  using namespace detail;

  return invoke_dispatch<8> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
                             ignore (), ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8,
  const T9&      arg9)
{
  using namespace detail;

  return invoke_dispatch<9> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ignore (), xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline Ret invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8,
  const T9&      arg9,
  const T10&     arg10)
{
  using namespace detail;

  return invoke_dispatch<10> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, xtl::type<Ret> ());
}

/*
    Вызов шлюза для void возвращаемого значения
*/

inline void invoke (IInterpreter& interpreter, const char* fn_name)
{
  invoke<void> (interpreter, fn_name);
}

template <class T1>
inline void invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1)
{
  invoke<void> (interpreter, fn_name, arg1);
}

template <class T1, class T2>
inline void invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2)
{
  invoke<void> (interpreter, fn_name, arg1, arg2);
}

template <class T1, class T2, class T3>
inline void invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2, const T3& arg3)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3);
}

template <class T1, class T2, class T3, class T4>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4);
}

template <class T1, class T2, class T3, class T4, class T5>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5);
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8,
  const T9&      arg9)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void invoke
 (IInterpreter&  interpreter,
  const char*    fn_name,
  const T1&      arg1,
  const T2&      arg2,
  const T3&      arg3,
  const T4&      arg4,
  const T5&      arg5,
  const T6&      arg6,
  const T7&      arg7,
  const T8&      arg8,
  const T9&      arg9,
  const T10&     arg10)
{
  invoke<void> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}
