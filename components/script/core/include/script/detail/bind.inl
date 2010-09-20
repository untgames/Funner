/*
    Создание аргумента функции-шлюза
*/

namespace detail
{

/*
    Извлечение аргументов из стека
*/

//общая версия
template <class T, bool is_enum=xtl::type_traits::is_enum<T>::value> struct common_argument_selector
{
  typedef T dump_type;

  static T get (IStack& stack, size_t index) { return xtl::any_multicast<T> (stack.GetVariant (index)); }  
};

//извлечение enum-типов
template <class T> struct common_argument_selector<T, true>
{
  typedef int dump_type;

  static T get (IStack& stack, size_t index) { return static_cast<T> (stack.GetInteger (index)); }  
};

//извлечение целочисленного аргумента
template <class T> struct int_argument_selector
{
  typedef int dump_type;

  static T get (IStack& stack, size_t index) { return static_cast<T> (stack.GetInteger (index)); }
};

//извлечение вещественного аргумента
template <class T> struct float_argument_selector
{
  typedef float dump_type;

  static T get (IStack& stack, size_t index) { return static_cast<T> (stack.GetFloat (index)); }
};

//извлечение void-указателей
struct raw_pointer_argument_selector
{
  typedef void* dump_type;

  static void* get (IStack& stack, size_t index) { return stack.GetPointer (index); }
};

//извлечение строк
struct string_argument_selector
{
  typedef const char* dump_type;

  static const char* get (IStack& stack, size_t index) { return stack.GetString (index); }
};

//извлечение вариантных типов данных
struct any_argument_selector
{
  typedef xtl::any& dump_type;

  static xtl::any& get (IStack& stack, size_t index) { return stack.GetVariant (index); }
};

//диспетчеризация взятия аргумента для различных типов данных
template <class T> struct argument_selector: public common_argument_selector<T> {};

template <> struct argument_selector<xtl::any>:                 public any_argument_selector {};
template <> struct argument_selector<const xtl::any>:           public any_argument_selector {};
template <> struct argument_selector<volatile xtl::any>:        public any_argument_selector {};
template <> struct argument_selector<const volatile xtl::any>:  public any_argument_selector {};
template <> struct argument_selector<xtl::any&>:                public any_argument_selector {};
template <> struct argument_selector<const xtl::any&>:          public any_argument_selector {};
template <> struct argument_selector<volatile xtl::any&>:       public any_argument_selector {};
template <> struct argument_selector<const volatile xtl::any&>: public any_argument_selector {};
template <> struct argument_selector<char>:                     public int_argument_selector<char> {};
template <> struct argument_selector<signed char>:              public int_argument_selector<signed char> {};
template <> struct argument_selector<unsigned char>:            public int_argument_selector<unsigned char> {};
template <> struct argument_selector<short>:                    public int_argument_selector<short> {};
template <> struct argument_selector<unsigned short>:           public int_argument_selector<unsigned short> {};
template <> struct argument_selector<int>:                      public int_argument_selector<int> {};
template <> struct argument_selector<unsigned int>:             public int_argument_selector<unsigned int> {};
template <> struct argument_selector<long>:                     public int_argument_selector<long> {};
template <> struct argument_selector<unsigned long>:            public int_argument_selector<unsigned long> {};
template <> struct argument_selector<float>:                    public float_argument_selector<float> {};
template <> struct argument_selector<double>:                   public float_argument_selector<double> {};
template <> struct argument_selector<long double>:              public float_argument_selector<long double> {};
template <> struct argument_selector<void*>:                    public raw_pointer_argument_selector {};
template <> struct argument_selector<const void*>:              public raw_pointer_argument_selector {};
template <> struct argument_selector<const volatile void*>:     public raw_pointer_argument_selector {};
template <> struct argument_selector<const char*>:              public string_argument_selector {};
template <> struct argument_selector<const volatile char*>:     public string_argument_selector {};

template <> struct argument_selector<bool>
{
  typedef bool dump_type;

  static bool get (IStack& stack, size_t index) { return stack.GetBoolean (index); }
};

template <class Traits, class Allocator>
struct argument_selector<stl::basic_string<char, Traits, Allocator> > : public string_argument_selector {};

/*
    Помещение аргумента в стек
*/

template <class T>
struct argument_invoker
{
  typedef xtl::any type;

  static type make (const T& value) { return xtl::make_ref_any (value); }  
};

template <class T, bool is_enum=xtl::type_traits::is_enum<T>::value>
struct argument_invoker_helper: public argument_invoker<T> {};

template <class T>
struct argument_invoker_helper<T, true>
{
    //enum-типы приводятся к int

  typedef int type;

  static int make (const T& value) { return static_cast<int> (value); }
};

template <class T>
inline typename argument_invoker_helper<T>::type make_invoker_argument (const T& value)
{
  return argument_invoker_helper<T>::make (value);
}

inline xtl::any    make_invoker_argument (const xtl::any& value)    { return value; }
inline bool        make_invoker_argument (bool value)               { return value; }
inline int         make_invoker_argument (char value)               { return value; }
inline int         make_invoker_argument (signed char value)        { return value; }
inline int         make_invoker_argument (unsigned char value)      { return value; }
inline int         make_invoker_argument (short value)              { return value; }
inline int         make_invoker_argument (unsigned short value)     { return value; }
inline int         make_invoker_argument (int value)                { return value; }
inline int         make_invoker_argument (unsigned int value)       { return value; }
inline int         make_invoker_argument (long value)               { return value; }
inline int         make_invoker_argument (unsigned long value)      { return value; }
inline float       make_invoker_argument (float value)              { return value; }
inline float       make_invoker_argument (double value)             { return static_cast<float> (value); }
inline float       make_invoker_argument (long double value)        { return static_cast<float> (value); }
inline void*       make_invoker_argument (void* value)              { return value; }
inline void*       make_invoker_argument (const void* value)        { return (void*)value; }
inline const char* make_invoker_argument (const char* string)       { return string; }
inline const char* make_invoker_argument (char* string)             { return string; }

template <unsigned int N> inline const char* make_invoker_argument (const char string [N]) { return &string [0]; }
template <unsigned int N> inline const char* make_invoker_argument (char string [N])       { return &string [0]; }
template <unsigned int N> inline const char* make_invoker_argument (const unsigned char string [N]) { return (const char*)&string [0]; }
template <unsigned int N> inline const char* make_invoker_argument (unsigned char string [N])       { return (const char*)&string [0]; }

template <class Traits, class Allocator>
inline const char* make_invoker_argument (const stl::basic_string<char, Traits, Allocator>& s)
{
  return s.c_str ();
}

template <class Traits, class Allocator>
inline const char* make_invoker_argument (stl::basic_string<char, Traits, Allocator>& s)
{
  return s.c_str ();
}

//взятие аргумента из стека
template <class T>
T get_argument (IStack& stack, size_t index)
{
  try
  {
    return argument_selector<T>::get (stack, index);
  }
  catch (script::ArgumentException&)
  {
    throw;
  }
  catch (xtl::bad_any_cast& exception)
  {
    throw xtl::format_exception<script::ArgumentException> ("script::detail::get_argument", "Exception on argument #%u: %s, %s -> %s\n", index, exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    throw xtl::format_exception<script::ArgumentException> ("script::detail::get_argument", "Exception on argument #%u: %s", index, exception.what ());
  }
  catch (...)
  {
    throw xtl::format_exception<script::ArgumentException> ("script::detail::get_argument", "Exception on argument #%u: unknown", index);
  }
}

//проверка наличия достаточного числа аргументов в стеке
template <size_t expected_arguments_count>
inline void check_arguments_count (size_t stack_arguments_count)
{
  if (stack_arguments_count < expected_arguments_count)
    throw xtl::format_exception<script::ArgumentException> ("script::detail::check_arguments_count", "Too few arguments (expected %u, got %u)", expected_arguments_count, stack_arguments_count);
}

/*
    Помещение аргумента в стек
*/

struct ignore {};

template <class T> inline void push_argument (IStack& stack, T& value)
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
  
  enum { argument_index = I + 1 };
};

template <class FunctionalTraits, size_t I>
struct functional_argument_traits<FunctionalTraits, I, true>
{
  typedef typename FunctionalTraits::template argument<I>::type argument_type;
  
  enum { argument_index = I + 1 };
};

template <class FunctionalTraits>
struct functional_argument_traits<FunctionalTraits, 0, true>
{
  typedef typename FunctionalTraits::object_type& argument_type;
  
  enum { argument_index = 1 };
};

//выборка аргумента
template <class FunctionalTraits> struct stack_argument_selector
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
  invoker_impl (const Fn& in_fn) : fn (in_fn) {}    

  size_t operator () (IStack& stack) const
  {
    enum { arguments_count = FnTraits::arguments_count + FnTraits::is_memfun };

      //проверка наличия достаточного числа аргументов в стеке    

    check_arguments_count<arguments_count> (stack.Size ());

      //вызов шлюза и помещение его результата в стек
      
    typename FnTraits::result_type result = xtl::apply<typename FnTraits::result_type, arguments_count> (fn, stack, xtl::make_const_ref (stack_argument_selector<FnTraits> ()));

    push_argument (stack, result);

    return 1; //results_count = 1
  }

  Fn fn;
};

template <class FnTraits, class Fn>
struct invoker_impl<FnTraits, Fn, void>
{
  invoker_impl (const Fn& in_fn) : fn (in_fn) {}

  size_t operator () (IStack& stack) const
  {
    enum { arguments_count = FnTraits::arguments_count + FnTraits::is_memfun };

      //проверка наличия достаточного числа аргументов в стеке    

    check_arguments_count<arguments_count> (stack.Size ());

      //вызов шлюза

    xtl::apply<void, arguments_count> (fn, stack, xtl::make_const_ref (stack_argument_selector<FnTraits> ()));

    return 0; //results_count = 0
  }

  Fn fn;
};

/*
    Дамп функции
*/

template <class T>
inline void dump_type (stl::string& buffer)
{
  buffer += typeid (T).name ();  
}

template <> inline void dump_type<int> (stl::string& buffer)
{
  buffer += "int";  
}

template <> inline void dump_type<bool> (stl::string& buffer)
{
  buffer += "bool";
}

template <> inline void dump_type<float> (stl::string& buffer)
{
  buffer += "float";
}

template <> inline void dump_type<const char*> (stl::string& buffer)
{
  buffer += "string";
}

template <> inline void dump_type<void*> (stl::string& buffer)
{
  buffer += "float";
}

template <class FunctionalTraits, size_t I, size_t Last> struct stack_argument_dumper
{
  static void dump (stl::string& buffer)
  {
    if (I) buffer += ", ";
  
    typedef typename functional_argument_traits<FunctionalTraits, I>::argument_type argument_type;

    dump_type<typename argument_selector<argument_type>::dump_type> (buffer);
    
    stack_argument_dumper<FunctionalTraits, I+1, Last>::dump (buffer);
  }
};

template <class FunctionalTraits, size_t I> struct stack_argument_dumper<FunctionalTraits, I, I>
{
  static void dump (stl::string& buffer) { buffer += ")"; }
};

//дамп функтора
template <class FnTraits, class Fn, class Ret>
inline void to_string (stl::string& buffer, const invoker_impl<FnTraits, Fn, Ret>& invoker)
{
  enum { arguments_count = FnTraits::arguments_count + FnTraits::is_memfun };
  
  buffer.clear ();
  
  dump_type<typename FnTraits::result_type> (buffer);
  
  buffer += " (";
  
  stack_argument_dumper<FnTraits, 0, arguments_count>::dump (buffer);
}

/*
    Функция-диспетчер вызова шлюза
*/

template <class Ret>
Ret get_result (IStack& stack)
{
  try
  {
    Ret result = get_argument<Ret> (stack, stack.Size () - 1);

    stack.Pop (1);

    return result;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::get_result");
    throw;
  }
}

template <> inline void get_result (IStack&)
{
}

template <class Ret> struct results_count       { enum { value = 1 }; };
template <>          struct results_count<void> { enum { value = 0 }; };

template <size_t ArgsCount, class Ret, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
Ret invoke_dispatch
 (IInterpreter& interpreter,
  const char*   name,
  T1&           arg1,
  T2&           arg2,
  T3&           arg3,
  T4&           arg4,
  T5&           arg5,
  T6&           arg6,
  T7&           arg7,
  T8&           arg8,
  T9&           arg9,
  T10&          arg10,
  xtl::type<Ret>)
{
  try
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

    interpreter.Invoke (ArgsCount, results_count<Ret>::value);

    return get_result<Ret> (stack);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::invoke(\"%s\",\"%s\",...)", interpreter.Name (), name);
    throw;
  }
}

}

/*
    Создание шлюза для произвольного функтора с известной сигнатурой
*/

template <class Signature, class Fn>
inline Invoker make_invoker (Fn fn)
{
  typedef detail::invoker_impl<xtl::functional_traits<Signature>, Fn> invoker_type;

  return Invoker (invoker_type (fn));
}

template <class Fn>
inline Invoker make_invoker (Fn fn)
{
  return make_invoker<Fn, Fn> (fn);
}

/*
    Создание функции, возвращающей константу
*/

namespace detail
{

template <class T> struct identity_functor
{
  T value;

  identity_functor (const T& in_value) : value (in_value) {}

  const T operator () () const { return value; }
};

}

template <class T>
inline Invoker make_const (const T& value)
{
  return make_invoker<T ()> (detail::identity_functor<T> (value));
}

/*
    Вызов шлюза
*/

template <class Ret>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name)
{
  using namespace detail;
  
  static ignore dummy;

  return invoke_dispatch<0> (interpreter, fn_name, dummy, dummy, dummy, dummy, dummy,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
}

template <class Ret, class T1>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1)
{
  using namespace detail;
  
  static ignore dummy;

  return invoke_dispatch<1> (interpreter, fn_name, arg1, dummy, dummy, dummy, dummy,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
}

template <class Ret, class T1, class T2>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2)
{
  using namespace detail;
  
  static ignore dummy;

  return invoke_dispatch<2> (interpreter, fn_name, arg1, arg2, dummy, dummy, dummy,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
}

template <class Ret, class T1, class T2, class T3>
inline Ret invoke (IInterpreter& interpreter, const char* fn_name, const T1& arg1, const T2& arg2, const T3& arg3)
{
  using namespace detail;
  
  static ignore dummy;

  return invoke_dispatch<3> (interpreter, fn_name, arg1, arg2, arg3, dummy, dummy,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<4> (interpreter, fn_name, arg1, arg2, arg3, arg4, dummy,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<5> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5,
                             dummy, dummy, dummy, dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<6> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6,
                             dummy, dummy, dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<7> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7,
                             dummy, dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<8> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
                             dummy, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<9> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, dummy, xtl::type<Ret> ());
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
  
  static ignore dummy;

  return invoke_dispatch<10> (interpreter, fn_name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, xtl::type<Ret> ());
}

/*
    Создание шлюза для функции обратного вызова
*/

namespace detail
{

template <class Ret> class callback_dispatcher
{
  typedef xtl::com_ptr<ISymbol> SymbolPtr;
  public:
    callback_dispatcher (IInterpreter* in_interpreter, const SymbolPtr& in_symbol)
      : interpreter (in_interpreter), symbol (in_symbol) {}
      
    const char* symbol_name () const { return symbol->Name (); }
            
    Ret operator () ()
    {    
      return invoke<0> (dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy);
    }
    
    template <class T1>
    Ret operator () (T1& arg1)
    {
      return invoke<1> (arg1, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy);
    }

    template <class T1, class T2>
    Ret operator () (T1& arg1, T2& arg2)
    {
      return invoke<2> (arg1, arg2, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy);
    }
    
    template <class T1, class T2, class T3>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3)
    {
      return invoke<3> (arg1, arg2, arg3, dummy, dummy, dummy, dummy, dummy, dummy, dummy);
    }
    
    template <class T1, class T2, class T3, class T4>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4)
    {
      return invoke<4> (arg1, arg2, arg3, arg4, dummy, dummy, dummy, dummy, dummy, dummy);
    }

    template <class T1, class T2, class T3, class T4, class T5>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5)
    {
      return invoke<5> (arg1, arg2, arg3, arg4, arg5, dummy, dummy, dummy, dummy, dummy);
    }
    
    template <class T1, class T2, class T3, class T4, class T5, class T6>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6)
    {
      return invoke<6> (arg1, arg2, arg3, arg4, arg5, arg6, dummy, dummy, dummy, dummy);
    }
    
    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7)
    {
      return invoke<7> (arg1, arg2, arg3, arg4, arg5, arg6, arg7, dummy, dummy, dummy);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8)
    {
      return invoke<8> (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, dummy, dummy);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9)
    {
      return invoke<9> (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, dummy);
    }

    template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    Ret operator () (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, T10& arg10)
    {
      return invoke<10> (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }

  private:
    template <size_t ArgsCount, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    Ret invoke (T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, T10& arg10)
    {
      if (!interpreter)
        throw xtl::format_operation_exception ("script::callback_dispatcher::invoke", "Interpreter has already destroyed");

      return detail::invoke_dispatch<ArgsCount> (*interpreter, symbol->Name (), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
        arg9, arg10, xtl::type<Ret> ());
    }

  private:
    typedef xtl::trackable_ptr<IInterpreter> InterpreterPtr;

  private:
    InterpreterPtr interpreter;
    SymbolPtr      symbol;
    
    static ignore dummy;
};

template <class Ret> detail::ignore callback_dispatcher<Ret>::dummy;

template <class Signature> struct callback_invoker
{
  size_t operator () (IStack& stack) const
  {
      //проверка наличия достаточного числа аргументов в стеке    

    check_arguments_count<1> (stack.Size ());  
  
    IInterpreter* interpreter = &stack.Interpreter ();

    xtl::com_ptr<ISymbol> symbol (stack.GetSymbol (1), false);
    
    if (!symbol)
      throw xtl::format_operation_exception ("script::callback_invoker::operator ()", "Null callback function");

    typedef typename xtl::functional_traits<Signature>::result_type result_type;

    xtl::function<Signature> result = callback_dispatcher<result_type> (interpreter, symbol);    

    stack.Push (xtl::make_ref_any (result));    

    return 1;
  }
};

}

template <class Signature>
inline Invoker make_callback_invoker ()
{
  return Invoker (detail::callback_invoker<Signature> ());
}
