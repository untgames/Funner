/*
    Адаптеры вызовов
*/

template <class Arg,class Ret>
inline Ret call (Ret (*fn)(Arg),typename pointer_to_unary_function<Arg,Ret>::argument_type x)
{
  return fn (x);
}

template <class Arg1,class Arg2,class Ret> 
inline Ret call
 (Ret (*fn)(Arg1,Arg2),
  typename pointer_to_binary_function<Arg1,Arg2,Ret>::first_argument_type  a,
  typename pointer_to_binary_function<Arg1,Arg2,Ret>::second_argument_type b)
{
  return fn (a,b);
}

template <class Arg1,class Arg2,class Arg3,class Ret>
inline Ret call
 (Ret (*fn)(Arg1,Arg2,Arg3),
  typename pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret>::first_argument_type  a,
  typename pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret>::second_argument_type b,
  typename pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret>::third_argument_type  c)
{
  return fn (a,b,c);
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline Ret call
 (Ret (*fn)(Arg1,Arg2,Arg3,Arg4),
  typename pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret>::first_argument_type  a,
  typename pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret>::second_argument_type b,
  typename pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret>::third_argument_type  c,
  typename pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret>::fourth_argument_type d)
{
  return fn (a,b,c,d);
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
inline Ret call
 (Ret (*fn)(Arg1,Arg2,Arg3,Arg4,Arg5),
  typename pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::first_argument_type  a,
  typename pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::second_argument_type b,
  typename pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::third_argument_type  c,
  typename pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::fourth_argument_type d,
  typename pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::fifth_argument_type  e)
{
  return fn (a,b,c,d,e);
}

template <class T,class Ret> 
inline Ret call (Ret (T::*fn)(),T* x)
{
  return (x->*fn)(); 
}

template <class T,class Ret> 
inline Ret call (Ret (T::*fn)() const,const T* x)
{
  return (x->*fn)();
}

template <class T,class Ret>
inline Ret call (Ret (T::*fn)(),T& x)
{
  return (x.*fn)();
}

template <class T,class Ret>
inline Ret call (Ret (T::*fn)() const,const T& x)
{
  return (x.*fn)();
}

template <class T,class Arg,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg),
  T*  a,
  typename mem_fun1_t<Ret,T,Arg>::second_argument_type b)
{
  return (a->*fn)(b);
}

template <class T,class Arg,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg) const,
  const T* a,
  typename const_mem_fun1_t<Ret,T,Arg>::second_argument_type b)
{
  return (a->*fn)(b);
}

template <class T,class Arg,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg),
  T&  a,
  typename mem_fun1_ref_t<Ret,T,Arg>::second_argument_type b)
{
  return (a.*fn)(b);
}

template <class T,class Arg,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg) const,
  const T& a,
  typename const_mem_fun1_ref_t<Ret,T,Arg>::second_argument_type b)
{
  return (a.*fn)(b);
}

template <class T,class Arg1,class Arg2,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2),
  T*  a,
  typename mem_fun2_t<Ret,T,Arg1,Arg2>::second_argument_type b,
  typename mem_fun2_t<Ret,T,Arg1,Arg2>::third_argument_type  c)
{
  return (a->*fn)(b,c);
}

template <class T,class Arg1,class Arg2,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2) const,
  const T* a,
  typename const_mem_fun2_t<Ret,T,Arg1,Arg2>::second_argument_type b,
  typename const_mem_fun2_t<Ret,T,Arg1,Arg2>::third_argument_type  c)
{
  return (a->*fn)(b,c);
}

template <class T,class Arg1,class Arg2,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2),
  T&  a,
  typename mem_fun2_ref_t<Ret,T,Arg1,Arg2>::second_argument_type b,
  typename mem_fun2_ref_t<Ret,T,Arg1,Arg2>::third_argument_type  c)
{
  return (a.*fn)(b,c);
}

template <class T,class Arg1,class Arg2,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2) const,
  const T& a,
  typename const_mem_fun2_ref_t<Ret,T,Arg1,Arg2>::second_argument_type b,
  typename const_mem_fun2_ref_t<Ret,T,Arg1,Arg2>::third_argument_type  c) 
{
  return (a.*fn)(b,c);
}

template <class T,class Arg1,class Arg2,class Arg3,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2,Arg3),
  T*  a,
  typename mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::second_argument_type b,
  typename mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::third_argument_type  c,
  typename mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::fourth_argument_type d)
{
  return (a->*fn)(b,c,d);
}

template <class T,class Arg1,class Arg2,class Arg3,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2,Arg3) const,
  const T* a,
  typename const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::second_argument_type b,
  typename const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::third_argument_type  c,
  typename const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::fourth_argument_type d)
{
  return (a->*fn)(b,c,d);
}

template <class T,class Arg1,class Arg2,class Arg3,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2,Arg3),
  T&  a,
  typename mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::second_argument_type b,
  typename mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::third_argument_type  c,
  typename mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::fourth_argument_type d)
{
  return (a.*fn)(b,c,d);
}

template <class T,class Arg1,class Arg2,class Arg3,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2,Arg3) const,
  const T& a,
  typename const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::second_argument_type b,
  typename const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::third_argument_type  c,
  typename const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::fourth_argument_type d)
{
  return (a.*fn)(b,c,d);
}

template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4),
  T*  a,
  typename mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::second_argument_type b,
  typename mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::third_argument_type  c,
  typename mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fourth_argument_type d,
  typename mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fifth_argument_type  e)
{
  return (a->*fn)(b,c,d,e);
}

template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2,Arg3,Arg4) const,
  const T* a,
  typename const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::second_argument_type b,
  typename const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::third_argument_type  c,
  typename const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fourth_argument_type d,
  typename const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fifth_argument_type  e)
{
  return (a->*fn)(b,c,d,e);
}

template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline Ret call
 (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4),
  T&  a,
  typename mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::second_argument_type b,
  typename mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::third_argument_type  c,
  typename mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fourth_argument_type d,
  typename mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fifth_argument_type  e)
{
  return (a.*fn)(b,c,d,e);
}

template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline Ret call
 (Ret      (T::*fn)(Arg1,Arg2,Arg3,Arg4) const,
  const T& a,
  typename const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::second_argument_type b,
  typename const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::third_argument_type  c,
  typename const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fourth_argument_type d,
  typename const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::fifth_argument_type  e)
{
  return (a.*fn)(b,c,d,e);
}

/*
    Базовые классы
*/

template <class Arg,class Ret,class Fn>
inline unary_recall<Arg,Ret,Fn>::unary_recall (function _fn)
  : fn (_fn)
  { }

template <class Arg,class Ret,class Fn>
inline Ret unary_recall<Arg,Ret,Fn>::operator () (Arg a) const
{
  return call (fn,a);
}

template <class Arg,class Ret,class Fn>
inline bool unary_recall<Arg,Ret,Fn>::operator == (const unary_recall& a) const
{
  return fn == a.fn;
}

template <class Arg,class Ret,class Fn>
inline bool unary_recall<Arg,Ret,Fn>::operator != (const unary_recall& a) const
{
  return fn != a.fn;
}

template <class Arg1,class Arg2,class Ret,class Fn>
inline binary_recall<Arg1,Arg2,Ret,Fn>::binary_recall (function _fn)
  : fn (_fn)
  { }

template <class Arg1,class Arg2,class Ret,class Fn>
inline Ret binary_recall<Arg1,Arg2,Ret,Fn>::operator () (Arg1 a,Arg2 b) const
{
  return call (fn,a,b);
}

template <class Arg1,class Arg2,class Ret,class Fn>
inline bool binary_recall<Arg1,Arg2,Ret,Fn>::operator == (const binary_recall& a) const
{
  return fn == a.fn;
}

template <class Arg1,class Arg2,class Ret,class Fn>
inline bool binary_recall<Arg1,Arg2,Ret,Fn>::operator != (const binary_recall& a) const
{
  return fn != a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Ret,class Fn>
inline ternary_recall<Arg1,Arg2,Arg3,Ret,Fn>::ternary_recall (function _fn)
  : fn (_fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Ret,class Fn>
inline Ret ternary_recall<Arg1,Arg2,Arg3,Ret,Fn>::operator () (Arg1 a,Arg2 b,Arg3 c) const
{
  return call (fn,a,b,c);
}

template <class Arg1,class Arg2,class Arg3,class Ret,class Fn>
inline bool ternary_recall<Arg1,Arg2,Arg3,Ret,Fn>::operator == (const ternary_recall& a) const
{
  return fn == a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Ret,class Fn>
inline bool ternary_recall<Arg1,Arg2,Arg3,Ret,Fn>::operator != (const ternary_recall& a) const
{
  return fn != a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Fn>
inline quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Fn>::quaternary_recall (function _fn)
  : fn (_fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Fn>
inline Ret quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Fn>::operator () (Arg1 a,Arg2 b,Arg3 c,Arg4 d) const
{
  return call (fn,a,b,c,d);
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Fn>
inline bool quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Fn>::operator == (const quaternary_recall& a) const
{
  return fn == a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Fn>
inline bool quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Fn>::operator != (const quaternary_recall& a) const
{
  return fn != a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Fn>
inline pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Fn>::pentanary_recall (function _fn)
  : fn (_fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Fn>
inline Ret pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Fn>::operator () (Arg1 a,Arg2 b,Arg3 c,Arg4 d,Arg5 e) const
{
  return call (fn,a,b,c,d,e);
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Fn>
inline bool pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Fn>::operator == (const pentanary_recall& a) const
{
  return fn == a.fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Fn>
inline bool pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Fn>::operator != (const pentanary_recall& a) const
{
  return fn != a.fn;
}

/*
    Производные классы
*/

template <class Arg,class Ret>
inline pointer_to_unary_function<Arg,Ret>::pointer_to_unary_function (Ret (*fn)(Arg))
  : unary_recall<Arg,Ret,Ret (*)(Arg)> (fn)
  { }

template <class Arg1,class Arg2,class Ret>
inline pointer_to_binary_function<Arg1,Arg2,Ret>::pointer_to_binary_function (Ret (*fn)(Arg1,Arg2))
  : binary_recall<Arg1,Arg2,Ret,Ret (*)(Arg1,Arg2)> (fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Ret>
inline pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret>::pointer_to_ternary_function (Ret (*fn)(Arg1,Arg2,Arg3))
  : ternary_recall<Arg1,Arg2,Arg3,Ret,Ret (*)(Arg1,Arg2,Arg3)> (fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret>::pointer_to_quaternary_function (Ret (*fn)(Arg1,Arg2,Arg3,Arg4))
  : quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Ret (*)(Arg1,Arg2,Arg3,Arg4)> (fn)
  { }

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
inline pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret>::pointer_to_pentanary_function (Ret (*fn)(Arg1,Arg2,Arg3,Arg4,Arg5))
  : pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5)> (fn)
  { }

template <class Ret,class T>
inline mem_fun_t<Ret,T>::mem_fun_t (Ret (T::*fn)())
  : unary_recall<T*,Ret,Ret (T::*)()> (fn)
  { }

template <class Ret,class T>
inline const_mem_fun_t<Ret,T>::const_mem_fun_t (Ret (T::*fn)() const)
  : unary_recall<const T*,Ret,Ret (T::*)() const> (fn)
  { }

template <class Ret,class T>
inline mem_fun_ref_t<Ret,T>::mem_fun_ref_t (Ret (T::*fn)())
  : unary_recall<T&,Ret,Ret (T::*)()> (fn)
  { }

template <class Ret,class T>
inline const_mem_fun_ref_t<Ret,T>::const_mem_fun_ref_t (Ret (T::*fn)() const)
  : unary_recall<const T&,Ret,Ret (T::*)() const> (fn)
  { }

template <class Ret,class T,class Arg>
inline mem_fun1_t<Ret,T,Arg>::mem_fun1_t (Ret (T::*fn)(Arg))
  : binary_recall<T*,Arg,Ret,Ret (T::*)(Arg)> (fn)
  { }

template <class Ret,class T,class Arg>
inline const_mem_fun1_t<Ret,T,Arg>::const_mem_fun1_t (Ret (T::*fn)(Arg) const)
  : binary_recall<const T*,Arg,Ret,Ret (T::*)(Arg) const> (fn)
  { }

template <class Ret,class T,class Arg>
inline mem_fun1_ref_t<Ret,T,Arg>::mem_fun1_ref_t (Ret (T::*fn)(Arg))
  : binary_recall<T&,Arg,Ret,Ret (T::*)(Arg)> (fn)
  { }

template <class Ret,class T,class Arg>
inline const_mem_fun1_ref_t<Ret,T,Arg>::const_mem_fun1_ref_t (Ret (T::*fn)(Arg) const)
  : binary_recall<const T&,Arg,Ret,Ret (T::*)(Arg) const> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2>
inline mem_fun2_t<Ret,T,Arg1,Arg2>::mem_fun2_t (Ret (T::*fn)(Arg1,Arg2))
  : ternary_recall<T*,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2>
inline const_mem_fun2_t<Ret,T,Arg1,Arg2>::const_mem_fun2_t (Ret (T::*fn)(Arg1,Arg2) const)
  : ternary_recall<const T*,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2) const> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2>
inline mem_fun2_ref_t<Ret,T,Arg1,Arg2>::mem_fun2_ref_t (Ret (T::*fn)(Arg1,Arg2))
  : ternary_recall<T&,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2>
inline const_mem_fun2_ref_t<Ret,T,Arg1,Arg2>::const_mem_fun2_ref_t (Ret (T::*fn)(Arg1,Arg2) const)
  : ternary_recall<const T&,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2) const> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::mem_fun3_t (Ret (T::*fn)(Arg1,Arg2,Arg3))
  : quaternary_recall<T*,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3>::const_mem_fun3_t (Ret (T::*fn)(Arg1,Arg2,Arg3) const)
  : quaternary_recall<const T*,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3) const> (fn)
  { }
  
template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::mem_fun3_ref_t (Ret (T::*fn)(Arg1,Arg2,Arg3))
  : quaternary_recall<T&,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3>::const_mem_fun3_ref_t (Ret (T::*fn)(Arg1,Arg2,Arg3) const)
  : quaternary_recall<const T&,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3) const> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::mem_fun4_t (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4))
  : pentanary_recall<T*,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::const_mem_fun4_t (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4) const)
  : pentanary_recall<const T*,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::mem_fun4_ref_t (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4))
  : pentanary_recall<T&,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4)> (fn)
  { }

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4>::const_mem_fun4_ref_t (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4) const)
  : pentanary_recall<const T&,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const> (fn)
  { }

/*
    "Конструкторы" адаптеров вызова
*/

template <class Arg,class Ret>
inline pointer_to_unary_function<Arg,Ret> ptr_fun (Ret (*fn)(Arg))
{
  return fn;
}

template <class Arg1,class Arg2,class Ret>
inline pointer_to_binary_function<Arg1,Arg2,Ret> ptr_fun (Ret (*fn)(Arg1,Arg2))
{
  return fn;
}

template <class Arg1,class Arg2,class Arg3,class Ret>
inline pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret> ptr_fun (Ret (*fn)(Arg1,Arg2,Arg3))
{
  return fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
inline pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret> ptr_fun (Ret (*fn)(Arg1,Arg2,Arg3,Arg4))
{
  return fn;
}

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
inline pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret> ptr_fun (Ret (*fn)(Arg1,Arg2,Arg3,Arg4,Arg5))
{
  return fn;
}

template <class Ret,class T>
inline mem_fun_t<Ret,T> mem_fun (Ret (T::*fn)())
{
  return fn;
}

template <class Ret,class T>
inline const_mem_fun_t<Ret,T> mem_fun (Ret (T::*fn)() const)
{
  return fn;
}

template <class Ret,class T,class Arg>
inline mem_fun1_t<Ret,T,Arg> mem_fun (Ret (T::*fn)(Arg))
{
  return fn;
}

template <class Ret,class T,class Arg>
inline const_mem_fun1_t<Ret,T,Arg> mem_fun (Ret (T::*fn)(Arg) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2>
inline mem_fun2_t<Ret,T,Arg1,Arg2> mem_fun (Ret (T::*fn)(Arg1,Arg2))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2>
inline const_mem_fun2_t<Ret,T,Arg1,Arg2> mem_fun (Ret (T::*fn)(Arg1,Arg2) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline mem_fun3_t<Ret,T,Arg1,Arg2,Arg3> mem_fun (Ret (T::*fn)(Arg1,Arg2,Arg3))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3> mem_fun (Ret (T::*fn)(Arg1,Arg2,Arg3) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4) const)
{
  return fn;
}

template <class Ret,class T>
inline mem_fun_ref_t<Ret,T> mem_fun_ref (Ret (T::*fn)())
{
  return fn;
}

template <class Ret,class T>
inline const_mem_fun_ref_t<Ret,T> mem_fun_ref (Ret (T::*fn)() const)
{
  return fn;
}

template <class Ret,class T,class Arg>
inline mem_fun1_ref_t<Ret,T,Arg> mem_fun_ref (Ret (T::*fn)(Arg))
{
  return fn;
}

template <class Ret,class T,class Arg>
inline const_mem_fun1_ref_t<Ret,T,Arg> mem_fun_ref (Ret (T::*fn)(Arg) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2>
inline mem_fun2_ref_t<Ret,T,Arg1,Arg2> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2>
inline const_mem_fun2_ref_t<Ret,T,Arg1,Arg2> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2,Arg3))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
inline const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2,Arg3) const)
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4))
{
  return fn;
}

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
inline const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun_ref (Ret (T::*fn)(Arg1,Arg2,Arg3,Arg4) const)
{
  return fn;
}
