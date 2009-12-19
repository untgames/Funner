namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подстановки аргументов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Args>
inline T& eval_argument (T& x, Args&)
{
  return x;
}

template <class T, class Args>
inline T& eval_argument (const reference_wrapper<T>& x, Args&)
{
  return x.get ();
}

template <class T, class Args>
inline T& eval_argument (reference_wrapper<T>& x, Args&)
{
  return x.get ();
}

template <size_t I, class Args>
inline typename tuple_element<I-1, Args>::type eval_argument (const placeholders::argument<I>&, Args& args)
{
  return get<I-1> (args);
}

template <size_t I, class Args>
inline typename tuple_element<I-1, Args>::type eval_argument (placeholders::argument<I>&, Args& args)
{
  return get<I-1> (args);
}

template <class Ret, class Fn, class Args, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>::result_type
eval_argument (const binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>& fn, Args& args)
{
  return fn.eval (args);
}

template <class Ret, class Fn, class Args, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>::result_type
eval_argument (binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>& fn, Args& args)
{
  return fn.eval (args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртка для подстановки результата вложенных bind-выражений, не являющихся ссылками
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> inline T& wrap_result (T& r)
{
  return r;
}

template <class T> inline const T& wrap_result (const T& r)
{
  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызовы bind
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs&, Args&, int_constant<0>)
{
  return funcall<Ret> (fn);
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<1>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<2>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<3>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<4>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<5>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<6>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<7>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<8>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)),
                           wrap_result (eval_argument (get<7> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, int_constant<9>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)),
                           wrap_result (eval_argument (get<7> (binded_args), args)),
                           wrap_result (eval_argument (get<8> (binded_args), args)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Операторы сравнения / логические отрицание
///////////////////////////////////////////////////////////////////////////////////////////////////
struct bind_equals      { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a == b; } };
struct bind_not_equals  { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a != b; } };
struct bind_less        { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a < b; } };
struct bind_greater     { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a > b; } };
struct bind_not_less    { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a >= b; } };
struct bind_not_greater { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a <= b; } };
struct bind_logical_not { template <class T> bool operator () (const T& a) const { return !a; } };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа возвращаемого значения class members
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4180) // qualifier applied to a function type has no meaning
#endif

template <class T, int I>   struct bind_add_cref;
template <class M, class T> struct bind_add_cref<M T::*, 0> { typedef M         type; };
template <class M, class T> struct bind_add_cref<M T::*, 1> { typedef M const & type; };

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

template <class T> struct bind_is_ref      { enum { value = 0 }; };
template <class T> struct bind_is_ref<T&>  { enum { value = 1 }; };
template <class T> struct bind_is_ref<T*>  { enum { value = 1 }; };

template <class Fn, class Arg, bool IsMemPtr = !(functional_traits<Fn>::is_function || functional_traits<Fn>::is_memfun || functional_traits<Fn>::is_ptrfun)>
struct bind_result_of_dispatch
{
  typedef typename result_of<Fn>::type type;
};

template <class M, class T, class Arg> struct bind_result_of_dispatch<M T::*, Arg, true>
{
  typedef typename bind_add_cref<M T::*, 1>::type type;
};

template <class M, class T, class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct bind_result_of_dispatch<M T::*, binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>, true>
{
  typedef typename binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>::result_type nested_result_type;
  typedef typename bind_add_cref<M T::*, bind_is_ref<nested_result_type>::value>::type   type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа возвращаемого значения
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn, class A1> struct bind_result_of
{
  typedef Ret type;
};

template <class Fn, class A1> struct bind_result_of<unspecified_result, Fn, A1>
{
  typedef typename bind_result_of_dispatch<Fn, A1>::type type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртка над bind-выражениями
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct binder
{
  template <class Ret1, class Fn1, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  friend struct binder;

  typedef typename make_tuple_mapper<T1, T2, T3, T4, T5, T6, T7, T8, T9>::type binded_arguments_type;
  typedef typename access_traits<T1>::const_type arg1_type;
  typedef typename access_traits<T2>::const_type arg2_type;
  typedef typename access_traits<T3>::const_type arg3_type;
  typedef typename access_traits<T4>::const_type arg4_type;
  typedef typename access_traits<T5>::const_type arg5_type;
  typedef typename access_traits<T6>::const_type arg6_type;
  typedef typename access_traits<T7>::const_type arg7_type;
  typedef typename access_traits<T8>::const_type arg8_type;
  typedef typename access_traits<T9>::const_type arg9_type;
  public:
    typedef typename bind_result_of<Ret, Fn, arg1_type>::type result_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    binder (const Fn& in_fn) : fn (in_fn) {}   
    binder (const Fn& in_fn, arg1_type a1) : fn (in_fn), binded_arguments (a1) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2) : fn (in_fn), binded_arguments (a1, a2) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3) : fn (in_fn), binded_arguments (a1, a2, a3) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) : fn (in_fn), binded_arguments (a1, a2, a3, a4) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7, a8) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8, arg9_type a9) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7, a8, a9) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вычисление значения bind-выражения
///////////////////////////////////////////////////////////////////////////////////////////////////    
    template <class Args> result_type eval (Args& args)
    {
      return bind_call<result_type> (fn, binded_arguments, args, int_constant<binded_arguments_type::size> ());
    }

    template <class Args> result_type eval (Args& args) const
    {
      return bind_call<result_type> (fn, binded_arguments, args, int_constant<binded_arguments_type::size> ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызовы
///////////////////////////////////////////////////////////////////////////////////////////////////
     result_type operator () ()
     {
       return eval (make_const_ref (tie ()));
     }

     template <class A1> result_type operator () (A1& a1)
     {
       return eval (make_const_ref (tie (a1)));
     }

     template <class A1, class A2> result_type operator () (A1& a1, A2& a2)
     {
       return eval (make_const_ref (tie (a1, a2)));
     }

     template <class A1, class A2, class A3> result_type operator () (A1& a1, A2& a2, A3& a3)
     {
       return eval (make_const_ref (tie (a1, a2, a3)));
     }

     template <class A1, class A2, class A3, class A4> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4)));
     }

     template <class A1, class A2, class A3, class A4, class A5> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7, a8)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9)
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7, a8, a9)));
     }
     
     result_type operator () () const
     {
       return eval (make_const_ref (tie ()));
     }

     template <class A1> result_type operator () (A1& a1) const
     {
       return eval (make_const_ref (tie (a1)));
     }

     template <class A1, class A2> result_type operator () (A1& a1, A2& a2) const
     {
       return eval (make_const_ref (tie (a1, a2)));
     }

     template <class A1, class A2, class A3> result_type operator () (A1& a1, A2& a2, A3& a3) const
     {
       return eval (make_const_ref (tie (a1, a2, a3)));
     }

     template <class A1, class A2, class A3, class A4> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4)));
     }

     template <class A1, class A2, class A3, class A4, class A5> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7, a8)));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9) const
     {
       return eval (make_const_ref (tie (a1, a2, a3, a4, a5, a6, a7, a8, a9)));
     }
     
///////////////////////////////////////////////////////////////////////////////////////////////////
///Логическое отрицание
///////////////////////////////////////////////////////////////////////////////////////////////////
    binder<bool, bind_logical_not, binder> operator ! () const
    {
      return binder<bool, bind_logical_not, binder> (bind_logical_not (), *this);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Генерация новых bind-выражений на основе отношения между двумя существующими
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class A> binder<bool, bind_equals, binder, A> operator == (A arg) const
    {
      return binder<bool, bind_equals, binder, A> (bind_equals (), *this, arg);
    }

    template <class A> binder<bool, bind_not_equals, binder, A> operator != (A arg) const
    {
      return binder<bool, bind_not_equals, binder, A> (bind_not_equals (), *this, arg);
    }

    template <class A> binder<bool, bind_less, binder, A> operator < (A arg) const
    {
      return binder<bool, bind_less, binder, A> (bind_less (), *this, arg);
    }

    template <class A> binder<bool, bind_greater, binder, A> operator > (A arg) const
    {
      return binder<bool, bind_greater, binder, A> (bind_greater (), *this, arg);
    }

    template <class A> binder<bool, bind_not_greater, binder, A> operator <= (A arg) const
    {
      return binder<bool, bind_not_greater, binder, A> (bind_not_greater (), *this, arg);
    }

    template <class A> binder<bool, bind_not_less, binder, A> operator >= (A arg) const
    {
      return binder<bool, bind_not_less, binder, A> (bind_not_less (), *this, arg);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка равенства двух bind-выражений
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ret1, class Fn1, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    bool equal (const binder<Ret1, Fn1, A1, A2, A3, A4, A5, A6, A7, A8, A9>& f) const
    {
      return fn == f.fn && binded_arguments == f.binded_arguments;
    }

  private:
    Fn                    fn;
    binded_arguments_type binded_arguments;
};

/*
    Проверка равенства bind-выражений
*/

template <class Ret1, class Fn1, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19,
          class Ret2, class Fn2, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline bool function_equal
 (const binder<Ret1, Fn1, T11, T12, T13, T14, T15, T16, T17, T18, T19>& f1,
  const binder<Ret2, Fn2, T21, T22, T23, T24, T25, T26, T27, T28, T29>& f2)
{
  return f1.equal (f2);  
}

}

/*
    Создание bind-выражений
*/

template <class Fn>
inline detail::binder<detail::unspecified_result, Fn> bind (Fn fn)
{
  return detail::binder<detail::unspecified_result, Fn> (fn);  
}

template <class Fn, class T1>
inline detail::binder<detail::unspecified_result, Fn, T1> bind (Fn fn, T1 arg1)
{
  return detail::binder<detail::unspecified_result, Fn, T1> (fn, arg1);
}

template <class Fn, class T1, class T2>
inline detail::binder<detail::unspecified_result, Fn, T1, T2> bind (Fn fn, T1 arg1, T2 arg2)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2> (fn, arg1, arg2);
}

template <class Fn, class T1, class T2, class T3>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3> (fn, arg1, arg2, arg3);
}

template <class Fn, class T1, class T2, class T3, class T4>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4> (fn, arg1, arg2, arg3, arg4);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5> (fn, arg1, arg2, arg3, arg4, arg5);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6> (fn, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

template <class Ret, class Fn>
inline detail::binder<Ret, Fn> bind (Fn fn)
{
  return detail::binder<Ret, Fn> (fn);  
}

template <class Ret, class Fn, class T1>
inline detail::binder<Ret, Fn, T1> bind (Fn fn, T1 arg1)
{
  return detail::binder<Ret, Fn, T1> (fn, arg1);
}

template <class Ret, class Fn, class T1, class T2>
inline detail::binder<Ret, Fn, T1, T2> bind (Fn fn, T1 arg1, T2 arg2)
{
  return detail::binder<Ret, Fn, T1, T2> (fn, arg1, arg2);
}

template <class Ret, class Fn, class T1, class T2, class T3>
inline detail::binder<Ret, Fn, T1, T2, T3> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3)
{
  return detail::binder<Ret, Fn, T1, T2, T3> (fn, arg1, arg2, arg3);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline detail::binder<Ret, Fn, T1, T2, T3, T4> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4> (fn, arg1, arg2, arg3, arg4);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5> (fn, arg1, arg2, arg3, arg4, arg5);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6> (fn, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

/*
    Проверка свойств типа
*/

//является ли тип подстановщиком аргементов функционального объекта
template <class T>  struct is_placeholder: public false_type {};
template <size_t I> struct is_placeholder<placeholders::argument<I> >: public true_type {};

//является ли тип выражением, полученным после вызова bind
template <class T> struct is_bind_expression: public false_type {};
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct is_bind_expression<detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> >: public true_type {};
