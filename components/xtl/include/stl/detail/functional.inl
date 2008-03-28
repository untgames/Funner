/*
    Арифметичесие операции
*/

template <class Arg1,class Arg2,class Result>
inline Result plus<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a + b;
}

template <class Arg1,class Arg2,class Result>
inline Result minus<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a - b;
}

template <class Arg1,class Arg2,class Result>
inline Result multiplies<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a * b;
}

template <class Arg1,class Arg2,class Result>
inline Result divides<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a / b;
}

template <class Arg1,class Arg2,class Result>
inline Result modulus<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a % b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_plus<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a += b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_minus<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a -= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_multiplies<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a *= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_divides<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a /= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_modulus<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a %= b;
}

template <class Arg,class Result>
inline Result negate<Arg,Result>::operator () (const Arg& a) const
{
  return -a;
}

/*
    Логичесике операции
*/

template <class Arg1,class Arg2>
inline bool equal_to<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a == b;
}

template <class Arg1,class Arg2>
inline bool not_equal_to<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a != b;
}

template <class Arg1,class Arg2>
inline bool less<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a < b;
}

template <class Arg1,class Arg2>
inline bool greater<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a > b;
}

template <class Arg1,class Arg2>
inline bool less_equal<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a <= b;
}

template <class Arg1,class Arg2>
inline bool greater_equal<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a >= b;
}

template <class Arg1,class Arg2>
inline bool logical_and<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a && b;
}

template <class Arg1,class Arg2>
inline bool logical_or<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a || b;
}

template <class Arg>
inline bool logical_not<Arg>::operator () (const Arg& a) const
{
  return !a;
}

/*
    Поразрядные операции
*/

template <class Arg1,class Arg2,class Result>
inline Result radix_and<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a & b;
}

template <class Arg1,class Arg2,class Result>
inline Result radix_or<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a | b;
}

template <class Arg1,class Arg2,class Result>
inline Result radix_xor<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a ^ b;
}

template <class Arg,class Result>
inline Result radix_not<Arg,Result>::operator () (const Arg& a) const
{
  return ~a;
}

template <class Arg1,class Arg2,class Result>
inline Result radix_left_shift<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a << b;
}

template <class Arg1,class Arg2,class Result>
inline Result radix_right_shift<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  return a >> b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_radix_and<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a &= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_radix_or<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a |= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_radix_xor<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a ^= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_radix_left_shift<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a <<= b;
}

template <class Arg1,class Arg2,class Result>
inline Result assign_radix_right_shift<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a >>= b;
}

/*
    Присваивание
*/

template <class Arg1,class Arg2,class Result>
inline Result assign<Arg1,Arg2,Result>::operator () (Arg1& a,const Arg2& b) const
{
  return a = b;
}

/*
    Отрицание предиката
*/

template <class Predicate>
inline unary_negate<Predicate>::unary_negate (const Predicate& p)
  : pred (p)
{ }

template <class Predicate>
inline bool unary_negate<Predicate>::operator () (const argument_type& x) const
{
  return !pred (x);
}

template <class Predicate>
inline binary_negate<Predicate>::binary_negate (const Predicate& p)
  : pred (p)
{ }

template <class Predicate>
inline bool binary_negate<Predicate>::operator () (const first_argument_type& a,const second_argument_type& b) const
{
  return !pred (a,b);
}

template <class Predicate> 
inline unary_negate<Predicate> not1 (const Predicate& p)
{
  return p;
}

template <class Predicate> 
inline binary_negate<Predicate> not2 (const Predicate& p)
{
  return p;
}

/*
    Подстановки и проекции
*/

template <class Fn> 
inline binder1st<Fn>::binder1st (const Fn& _fn,const typename Fn::first_argument_type& _val)
  : fn (_fn), value (_val)
{ }

template <class Fn>    
inline typename binder1st<Fn>::result_type binder1st<Fn>::operator () (const argument_type& x) const
{
  return fn (value,x);
}

template <class Fn> 
inline binder2nd<Fn>::binder2nd (const Fn& _fn,const typename Fn::second_argument_type& _val)
  : fn (_fn), value (_val)
{ }

template <class Fn>    
inline typename binder2nd<Fn>::result_type binder2nd<Fn>::operator () (const argument_type& x) const
{
  return fn (x,value);
}

template <class Fn,class T> 
inline binder1st<Fn> bind1st (const Fn& fn,const T& val)
{
  return binder1st<Fn> (fn,val);
}

template <class Fn,class T> 
inline binder2nd<Fn> bind2nd (const Fn& fn,const T& val)
{
  return binder2nd<Fn> (fn,val);
}

template <class Pair>
inline const typename select1st<Pair>::result_type& select1st<Pair>::operator () (const argument_type& x) const
{
  return x.first;
}

template <class Pair>
inline const typename select2nd<Pair>::result_type& select2nd<Pair>::operator () (const argument_type& x) const
{
  return x.second;
}

template <class Arg1,class Arg2>
inline typename project1st<Arg1,Arg2>::result_type project1st<Arg1,Arg2>::operator () 
 (const first_argument_type& a,
  const second_argument_type&)
{
  return a;
}

template <class Arg1,class Arg2>
inline typename project2nd<Arg1,Arg2>::result_type project2nd<Arg1,Arg2>::operator () 
 (const first_argument_type&,
  const second_argument_type& b)
{
  return b;
}
 
template <class T>
inline const typename identity<T>::result_type& identity<T>::operator () (const argument_type& x) const
{
  return x;
}

template <class T>
inline constant_void_fun<T>::constant_void_fun (const result_type& _val)
  : val (_val)
{ }
     
template <class T>
inline const typename constant_void_fun<T>::result_type& constant_void_fun<T>::operator () () const
{
  return val;
}

template <class Result,class Arg>
inline constant_unary_fun<Result,Arg>::constant_unary_fun (const result_type& _val)
  : val (_val)
{ }
     
template <class Result,class Arg>
inline const Result& constant_unary_fun<Result,Arg>::operator () (const Arg&) const
{
  return val;
}

template <class Result,class Arg1,class Arg2>
inline constant_binary_fun<Result,Arg1,Arg2>::constant_binary_fun (const result_type& _val)
  : val (_val)
{ }

template <class Result,class Arg1,class Arg2>
inline const Result& constant_binary_fun<Result,Arg1,Arg2>::operator () (const Arg1&,const Arg2&) const
{
  return val;
}

template <class Ret> 
inline constant_void_fun<Ret> constant0 (const Ret& val)
{
  return val;
}

template <class Ret> 
inline constant_unary_fun<Ret,Ret> constant1 (const Ret& val)
{
  return val;
}

template <class Ret> 
inline constant_binary_fun<Ret,Ret,Ret> constant2 (const Ret& val)
{
  return val;
}

/*
    Суперпозиция функций
*/

template <class Fn1,class Fn2>
inline unary_compose<Fn1,Fn2>::unary_compose (const Fn1& _fn1,const Fn2& _fn2)
  : fn1 (_fn1), fn2 (_fn2)
{ }
    
template <class Fn1,class Fn2>
inline typename unary_compose<Fn1,Fn2>::result_type unary_compose<Fn1,Fn2>::operator () (const argument_type& x) const
{
  return fn1 (fn2 (x));
}
      
template <class Fn1,class Fn2,class Fn3>
inline binary_compose<Fn1,Fn2,Fn3>::binary_compose (const Fn1& _fn1,const Fn2& _fn2,const Fn3& _fn3)
  : fn1 (_fn1), fn2 (_fn2), fn3 (_fn3)
{ }

template <class Fn1,class Fn2,class Fn3>
inline typename binary_compose<Fn1,Fn2,Fn3>::result_type binary_compose<Fn1,Fn2,Fn3>::operator () (const argument_type& x) const
{
  return fn1 (fn2 (x),fn3 (x));
}
 
template <class Fn1,class Fn2>
inline unary_compose<Fn1,Fn2> compose1 (const Fn1& fn1,const Fn2& fn2)
{
  return unary_compose<Fn1,Fn2> (fn1,fn2);
}

template <class Fn1,class Fn2,class Fn3> 
inline binary_compose<Fn1,Fn2,Fn3> compose2 (const Fn1& fn1,const Fn2& fn2,const Fn3& fn3)
{
  return binary_compose<Fn1,Fn2,Fn3> (fn1,fn2,fn3);
}

/*
    Получение "начального" элемента для операций
*/

template <class Arg1,class Arg2,class Result>
inline Result identity_element (plus<Arg1,Arg2,Result>)
{
  return Result (0);
}

template <class Arg1,class Arg2,class Result>
inline Result identity_element (multiplies<Arg1,Arg2,Result>)
{
  return Result (1);
}
