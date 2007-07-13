/*
    Вспомогательные классы
*/

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа аргументов функции
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct function_argument                { typedef const T&      type; };
template <class T> struct function_argument<T&>            { typedef T&            type; };
template <>        struct function_argument<void_argument> { typedef void_argument type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовый класс для функций обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
struct function_invoker_base
{
  public:
            function_invoker_base () : ref_count (1) {}
    virtual ~function_invoker_base () {}  
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void release ()
    {
      if (!--ref_count)
        delete this;
    }

    void addref ()
    {
      ref_count++;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение типа хранимого функционального объекта и указателя на него
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const std::type_info& target_type () = 0;
    virtual void*                 target      () = 0;
  
  private:
    size_t ref_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс обработчиков вызова функторов для разного числа аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
//интерфейс обработчика вызова функтора без аргументов
template <class Ret>
struct function_invoker<Ret>: public function_invoker_base
{
  virtual Ret operator () () = 0;
};

//интерфейс обработчика вызова функтора с 1-м аргументом
template <class T, class Ret>
struct function_invoker<Ret, T>: public function_invoker_base
{
  virtual Ret operator () (T) = 0;
};

//интерфейс обработчика вызова функтора с 2-мя аргументами
template <class T1, class T2, class Ret>
struct function_invoker<Ret, T1, T2>: public function_invoker_base
{
  virtual Ret operator () (T1, T2) = 0;
};

//интерфейс обработчика вызова функтора с 3-мя аргументами
template <class T1, class T2, class T3, class Ret>
struct function_invoker<Ret, T1, T2, T3>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3) = 0;
};

//интерфейс обработчика вызова функтора с 4-мя аргументами
template <class T1, class T2, class T3, class T4, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4) = 0;
};

//интерфейс обработчика вызова функтора с 5-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4, T5>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4, T5) = 0;
};

//интерфейс обработчика вызова функтора с 6-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4, T5, T6>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4, T5, T6) = 0;
};

//интерфейс обработчика вызова функтора с 7-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4, T5, T6, T7>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4, T5, T6, T7) = 0;
};

//интерфейс обработчика вызова функтора с 8-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4, T5, T6, T7, T8>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4, T5, T6, T7, T8) = 0;
};

//интерфейс обработчика вызова функтора с 9-ю аргументами
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class Ret>
struct function_invoker<Ret, T1, T2, T3, T4, T5, T6, T7, T8, T9>: public function_invoker_base
{
  virtual Ret operator () (T1, T2, T3, T4, T5, T6, T7, T8, T9) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Fn>
inline bool function_equals (const function<Signature>& a, Fn& b)
{
  const Fn* a_fn = a.target<Fn> ();
  
  return a_fn ? *a_fn == b : false;
}

template <class Signature, class Fn>
inline bool function_equals (Fn& a, const function<Signature>& b)
{
  const Fn* b_fn = b.target<Fn> ();
  
  return b_fn ? a == *b_fn : false;
}

template <class Signature, class Fn>
inline bool function_not_equals (const function<Signature>& a, Fn& b)
{
  const Fn* a_fn = a.target<Fn> ();
  
  return a_fn ? *a_fn != b : false;
}

template <class Signature, class Fn>
inline bool function_not_equals (Fn& a, const function<Signature>& b)
{
  const Fn* b_fn = b.target<Fn> ();
  
  return b_fn ? a != *b_fn : false;
}

template <class Signature>
inline bool function_equals (const function<Signature>& f, null_type)
{
  return f.empty ();
}

template <class Signature>
inline bool function_equals (null_type, const function<Signature>& f)
{
  return f.empty ();
}

template <class Signature>
inline bool function_not_equals (const function<Signature>& f, null_type)
{
  return !f.empty ();
}

template <class Signature>
inline bool function_not_equals (null_type, const function<Signature>& f)
{
  return !f.empty ();
}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик вызовов пустой функции обратного вызова
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> class function<Signature>::empty_invoker_impl: public invoker_type
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение типа хранимого функционального объекта и указателя на него
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& target_type () { return typeid (null_type); }
    void*                 target      () { return &null; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика для разного набора аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () ()
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type)
    {
      throw bad_function_call ();
    }

    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type)
    {
      throw bad_function_call ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение экземпляра обработчика пустого делегата
///////////////////////////////////////////////////////////////////////////////////////////////////
    static empty_invoker_impl* instance_ptr ()
    {
      static char buffer [sizeof (empty_invoker_impl)];
      static empty_invoker_impl* invoker = new (buffer) empty_invoker_impl;

      return invoker;
    }

  private:
    empty_invoker_impl () {}
    
  private:
    null_type null;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчик вызовов функтора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature> template <class Fn> class function<Signature>::invoker_impl: public invoker_type
{
  public:
    invoker_impl (Fn& in_fn) : fn (in_fn) { }        
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение типа хранимого функционального объекта и указателя на него
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& target_type () { return typeid (fn); }
    void*                 target      () { return (void*)&fn; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов обработчика для разного набора аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () ()
    {
      return funcall<result_type> (unwrap (fn));
    }

    result_type operator () (arg1_type a1)
    {
      return funcall<result_type> (unwrap (fn), a1);
    }

    result_type operator () (arg1_type a1, arg2_type a2)
    {
      return funcall<result_type> (unwrap (fn), a1, a2);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4, a5);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4, a5, a6);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4, a5, a6, a7);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4, a5, a6, a7, a8);
    }

    result_type operator () (arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8, arg9_type a9)
    {
      return funcall<result_type> (unwrap (fn), a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (Fn& f) const { return fn == f; }

  private:
    Fn fn;
};

/*
    function
*/

/*
    Конструкторы / деструктор
*/

template <class Signature>
inline function<Signature>::function ()
  : invoker (create_invoker (null_type ()))
  { }

template <class Signature>
inline function<Signature>::function (const function& f)
  : invoker (f.invoker)
{
  invoker->addref ();  
}

template <class Signature> template <class Fn>
inline function<Signature>::function (Fn f)
  : invoker (create_invoker (f))
  { }

template <class Signature> template <class Signature1>
inline function<Signature>::function (const function<Signature1>& f)
  : invoker (f ? create_invoker (f) : create_invoker (null_type ()))
  { }

template <class Signature>
inline function<Signature>::~function ()
{
  invoker->release ();
}

/*
    Присваивание
*/

template <class Signature>
inline function<Signature>& function<Signature>::operator = (const function& f)
{
  function (f).swap (*this);
  
  return *this;
}

template <class Signature> template <class Fn>
inline function<Signature>& function<Signature>::operator = (Fn f)
{
  function (f).swap (*this);

  return *this;
}

template <class Signature> template <class Signature1>
inline function<Signature>& function<Signature>::operator = (const function<Signature1>& f)
{
  function (f).swap (*this);

  return *this;
}

/*
    Очистка
*/

template <class Signature>
inline void function<Signature>::clear ()
{
  function ().swap (*this);
}

/*
    Создание специализированных обработчиков
*/

template <class Signature> template <class Fn>
inline typename function<Signature>::invoker_type* function<Signature>::create_invoker (Fn& f)
{
  typedef functional_traits<Fn> fn_traits_type;

  return create_invoker (f, mpl::bool_constant<fn_traits_type::is_memfun || fn_traits_type::is_ptrfun> ());
}

template <class Signature> template <class Fn>
inline typename function<Signature>::invoker_type* function<Signature>::create_invoker (const reference_wrapper<Fn>& f)
{
  typedef functional_traits<Fn> fn_traits_type;

  return create_invoker (f, mpl::bool_constant<fn_traits_type::is_memfun || fn_traits_type::is_ptrfun> ());
}

template <class Signature> template <class Fn>
inline typename function<Signature>::invoker_type* function<Signature>::create_invoker (Fn& f, mpl::true_type)
{
  return unwrap (f) != 0 ? create_invoker (f, mpl::false_type ()) : create_invoker (null_type ());
}

template <class Signature> template <class Fn>
inline typename function<Signature>::invoker_type* function<Signature>::create_invoker (Fn& f, mpl::false_type)
{
  return new invoker_impl<Fn> (f);
}

template <class Signature>
inline typename function<Signature>::invoker_type* function<Signature>::create_invoker (null_type)
{
  invoker_type* invoker = empty_invoker_impl::instance_ptr ();

  invoker->addref ();

  return invoker;
}

/*
    Проверка является ли данный делегат пустым
*/

template <class Signature>
inline bool function<Signature>::empty () const
{
  return invoker == empty_invoker_impl::instance_ptr ();
}

/*
    Определение типа хранимого функционального объекта и возвращение указателя на него
*/

template <class Signature>
inline const std::type_info& function<Signature>::target_type () const
{
  return invoker->target_type ();
}

template <class Signature> template <class T>
inline T* function<Signature>::target ()
{
  return &typeid (T) == &invoker->target_type () ? static_cast<T*> (invoker->target ()) : 0;
}

template <class Signature> template <class T>
inline const T* function<Signature>::target () const
{
  return &typeid (T) == &invoker->target_type () ? static_cast<const T*> (invoker->target ()) : 0;
}

/*
    Проверка хранит ли функция функциональный объект эквивалентный заданному
*/

template <class Signature> template <class Fn>
inline bool function<Signature>::contains (const Fn& f) const
{
  if (const Fn* this_fn = target<Fn> ())
  {
    return function_equal (*this_fn, f);
  }
  else if (const Fn* this_fn = target<const Fn> ())
  {
    return function_equal (*this_fn, f);
  }
  else return false;
}

/*
    Обмен
*/

template <class Signature>
inline void function<Signature>::swap (function& f)
{
  invoker_type* tmp = invoker;
  invoker           = f.invoker;
  f.invoker         = tmp;
}

template <class Signature>
inline void swap (function<Signature>& a, function<Signature>& b)
{
  a.swap (b);
}

/*
    Вызов делегата
*/

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () () const
{
  return (*invoker)();
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () (arg1_type a1) const
{
  return (*invoker)(a1);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator () (arg1_type a1, arg2_type a2) const
{
  return (*invoker)(a1, a2);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3) const
{
  return (*invoker)(a1, a2, a3);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1, 
  arg2_type a2, 
  arg3_type a3, 
  arg4_type a4) const
{
  return (*invoker)(a1, a2, a3, a4);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5) const
{
  return (*invoker)(a1, a2, a3, a4, a5);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6) const
{
  return (*invoker)(a1, a2, a3, a4, a5, a6);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7) const
{
  return (*invoker)(a1, a2, a3, a4, a5, a6, a7);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7,
  arg8_type a8) const
{
  return (*invoker)(a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class Signature>
inline typename function<Signature>::result_type function<Signature>::operator ()
 (arg1_type a1,
  arg2_type a2,
  arg3_type a3,
  arg4_type a4,
  arg5_type a5,
  arg6_type a6,
  arg7_type a7,
  arg8_type a8,
  arg9_type a9) const
{
  return (*invoker)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

/*
    Сравнение
*/

template <class Signature> template <class Fn>
inline bool function<Signature>::operator == (Fn fn) const
{
  return detail::function_equals (*this, unwrap (fn));
}

template <class Signature, class Fn>
inline bool operator == (Fn a, const function<Signature>& b)
{
  return detail::function_equals (unwrap (a), b);
}

template <class Signature> template <class Fn>
inline bool function<Signature>::operator != (Fn fn) const
{
  return detail::function_not_equals (*this, unwrap (fn));
}

template <class Signature, class Fn>
inline bool operator != (Fn a, const function<Signature>& b)
{
  return detail::function_not_equals (unwrap (a), b);
}
