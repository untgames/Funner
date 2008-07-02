#include <stdio.h>

namespace detail
{

template <class T, size_t SizeX, size_t SizeY> 
  const vec<T, SizeY>& get_component (const math::matrix<T, SizeX,SizeY>& v, size_t index);

template <class T, size_t SizeX, size_t SizeY> 
  vec<T, SizeY>& get_component (math::matrix<T,SizeX,SizeY>& v, size_t index);

template <class T, size_t Size> T& get_component (vec<T, Size>& v, size_t index);

template <class T, size_t Size> const T& get_component (const vec<T, Size>& v, size_t index);



template <class T> const T& get_component (const T& v, size_t)
{
  return v;
}

}

/*
	Общие функции для унарных и бинарных операций
*/
template <class Ret, class Fn> template <class T1> 
inline void unary_component_function<Ret,Fn>::operator () (Ret& res, const T1& a)
{
  for (size_t i=0; i<Ret::size; i++)
    res [i] = fn (detail::get_component (a, i));
}

template <class Ret, class Fn> template <class T1, class T2>
inline void binary_component_function<Ret,Fn>::operator () (Ret& res, const T1& a, const T2& b)
{
  for (size_t i=0; i<Ret::size; i++)
    res [i] = Fn() (detail::get_component (a, i), detail::get_component (b, i));
}

template <class Res> template <class T,class Fn1,class Fn2>
inline Res binary_accumulation_function<Res>::operator()(const T& a,const T& b,const Res& init,const Fn1& fn1,const Fn2& fn2)
{
  Res res(init);
  for (int i=0;i<T::size;i++)
  {
    res=fn1(res,fn2(detail::get_component(a,i),detail::get_component(b,i)));
  }
  return res;
}

template <class Fn1,class Fn2> template <class T,class T1>
inline T unary_accumulation_function<Fn1,Fn2>::operator()(const T1& a,T& init)
{
  T res=init;
  for (int i=0;i<T1::size;i++)
  {
    res=Fn1()(res,Fn2()(detail::get_component(a,i)));
  }
  return res;
}
template <class T,class Fn> template<class T1>
inline bool compare_function<T,Fn>::operator () (const T1& a,const T1& b,const T& eps)
{
  for(size_t i=0;i<T1::size;i++)
  {
    if (!Fn()(detail::get_component(a,i),detail::get_component(b,i),eps)) return false;
  }
  return true;
}

template <class Ret, class Fn, class T>
Ret make_unary_operation (const T& a,const Fn& fn)
{
  return Ret (a, unary_component_function<Ret, Fn> (fn));
}

template <class Ret, class T1, class T2, class Fn>
Ret make_binary_operation (const T1& a, const T2& b, Fn fn)
{
  return Ret (a, b, binary_component_function<Ret, Fn> (fn));
}

template <class Ret, class Fn, class T>
inline void make_unary_operation (const T& a,const Fn& fn, Ret& result)
{
  unary_component_function<Ret, Fn> MyFn(fn);
  MyFn(result, a);
}

template <class Ret, class T1, class T2, class Fn>
inline void make_binary_operation (const T1& a, const T2& b, Fn fn, Ret& result)
{
  binary_component_function<Ret, Fn> my_fn(fn);
  my_fn(result, a, b);
}


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


template <class Arg,class Result>
inline Result assign<Arg,Result>::operator () (const Arg& a) const
{
  return a;
}

template <class Arg,class Result>
inline Result negate<Arg,Result>::operator () (const Arg& a) const
{
  return -a;
}

/*
    Логические операции
*/

template <class Arg1,class Arg2,class Eps>
inline bool equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  if ((b>=a-eps)&&(b<=a+eps))
  {
    return 1;
  }
  return 0;
}

/*template <class Arg1,class Arg2,class Eps>
inline bool equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b) const
{
  return a==b;
}

template <class Arg1,class Arg2>
inline bool not_equal_to<Arg1,Arg2>::operator () (const Arg1& a,const Arg2& b) const
{
  return a != b;
}*/

template <class Arg1,class Arg2,class Eps>
inline bool not_equal_to<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  if ((b>=a-eps)&&(b<=a+eps))
  {
    return 0;
  }
  return 1;
}

//inline bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;

template <class Arg1,class Arg2,class Eps>
inline bool less<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a+eps) < b;
}

template <class Arg1,class Arg2,class Eps>
inline bool greater<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a-eps) > b;
}

template <class Arg1,class Arg2,class Eps>
inline bool less_equal<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a-eps) <= b;
}

template <class Arg1,class Arg2,class Eps>
inline bool greater_equal<Arg1,Arg2,Eps>::operator () (const Arg1& a,const Arg2& b,const Eps& eps) const
{
  return (a+eps) >= b;
}

template <class Arg,class Result>
inline Result absol<Arg,Result>:: operator () (const Arg&a) const
{
  if(a<Arg(0))
  {
    return -a;
  }
  return a;
}

template <class Arg1,class Arg2,class Result>
inline Result min_fn<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  if (a<b) return a;
  return b;
}

template <class Arg1,class Arg2,class Result>
inline Result max_fn<Arg1,Arg2,Result>::operator () (const Arg1& a,const Arg2& b) const
{
  if (a>b) return a;
  return b;
}

