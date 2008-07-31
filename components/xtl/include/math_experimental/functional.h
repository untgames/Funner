#ifndef MATHLIB_FUNCTIONAL_HEADER
#define MATHLIB_FUNCTIONAL_HEADER
#include <math_experimental/forward.h>

#ifdef _MSC_VER  
  #pragma pack(push,1)
#endif

namespace math
{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Применение функционалов для векторов
/////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> 
struct scalar
{
  scalar (T in_value) : value (in_value) {}

  T value;  
};

//template <class T> T& get_component (T& v, size_t);
namespace detail
{
template <class T> const T& get_component (const T& v, size_t);
template <class T> const T& get_component (const point<T>& p,size_t i);
}


template <class Ret, class Fn> struct unary_component_function
{ 
//unary_component_function () {}
  unary_component_function (Fn in_fn) : fn (in_fn) {}

  template <class T1> void operator () (Ret& res, const T1& a);
  
  Fn fn;
};

template <class Ret, class Fn> struct binary_component_function
{
//binary_component_function (){}
  binary_component_function (Fn in_fn) : fn (in_fn) {}

  template <class T1, class T2> void operator () (Ret& res, const T1& a, const T2& b);
  
  Fn fn;
};



template <class Res>
class binary_accumulation_function
{
 public:
  template <class T,class Fn1,class Fn2>
   Res operator()(const T& a,const T& b,const Res& init,const Fn1& fn1,const Fn2& fn2);
};

template <class Fn1,class Fn2>
class unary_accumulation_function
{
 public:
  template <class T,class T1>
   T operator()(const T1& a,T& init);
};



template <class T,class Fn>
class compare_function
{
  public:
	template<class T1>
		bool operator () (const T1& a,const T1& b,const T& eps=T(0));
};



template <class Ret, class Fn, class T>
Ret make_unary_operation (const T& a,const Fn& fn);

template <class Ret, class T1, class T2, class Fn>
Ret make_binary_operation (const T1& a, const T2& b, Fn fn);

template <class Ret, class Fn, class T>
void make_unary_operation (const T& a,const Fn& fn, Ret& result);

template <class Ret, class T1, class T2, class Fn>
void make_binary_operation (const T1& a, const T2& b, Fn fn, Ret& result);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Определение одноместного функтора
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Result=Arg>
struct unary_function 
{
  typedef Arg    argument_type;
  typedef Result result_type;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Определение двуместного функтора
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Result=Arg1> 
struct binary_function 
{
  typedef Arg1   first_argument_type;
  typedef Arg2   second_argument_type;
  typedef Result result_type;
};

/*
    Арифметико-логические операции 
    Расширение: assign_* - операции вида *=, +=, etc 
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сложение
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct plus: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вычитание
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct minus: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Умножение
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct multiplies: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Деление
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct divides: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Result=Arg>
struct assign: public unary_function<Arg,Result>
{
  Result operator() (const Arg&) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Унарный минус
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Result=Arg>
struct negate: public unary_function<Arg,Result>
{
  Result operator () (const Arg&) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение равенства (==)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct equal_to: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение неравенства (!=)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct not_equal_to: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение "меньше" (<)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct less: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение "больше" (>)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct greater: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение "не больше" (<=)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct less_equal: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Отношение "не меньше" (>=)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2=Arg1,class Eps=Arg1> 
struct greater_equal: public binary_function<Arg1,Arg2,bool> 
{
  bool operator () (const Arg1& a,const Arg2& b,const Eps& eps=Eps(0)) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Модуль значения
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Result=Arg>
struct absol: public unary_function<Arg,Result>
{
  Result operator () (const Arg&) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Минимум
/////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct min_fn: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Максимум
/////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Arg1,class Arg2=Arg1,class Result=Arg1>
struct max_fn: public binary_function<Arg1,Arg2,Result>
{
  Result operator () (const Arg1&,const Arg2&) const;
};

#include<math_experimental/impl/functional.inl>

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}
#endif
