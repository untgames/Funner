#ifndef __MYSTL_ADAPTORS__
#define __MYSTL_ADAPTORS__

#include <stl/config.h>

namespace stl
{

//forwards
template <class MemberFunction> struct memfun_traits;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение указателя на константную функцию-член
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,class Ret> struct memfun_traits<Ret (T::*)()>
{
  typedef Ret (T::*function)();
  typedef Ret (T::*const_function)() const;
};

template <class T,class Ret> struct memfun_traits<Ret (T::*)() const>
{
  typedef Ret (T::*function)();
  typedef Ret (T::*const_function)() const;  
};

template <class T,class Ret,class Arg> struct memfun_traits<Ret (T::*)(Arg)>
{
  typedef Ret (T::*function)(Arg);
  typedef Ret (T::*const_function)(Arg) const;  
};

template <class T,class Ret,class Arg> struct memfun_traits<Ret (T::*)(Arg) const>
{
  typedef Ret (T::*function)(Arg);
  typedef Ret (T::*const_function)(Arg) const;  
};

template <class T,class Ret,class Arg1,class Arg2> struct memfun_traits<Ret (T::*)(Arg1,Arg2)>
{
  typedef Ret (T::*function)(Arg1,Arg2);
  typedef Ret (T::*const_function)(Arg1,Arg2) const;  
};

template <class T,class Ret,class Arg1,class Arg2> struct memfun_traits<Ret (T::*)(Arg1,Arg2) const>
{
  typedef Ret (T::*function)(Arg1,Arg2);
  typedef Ret (T::*const_function)(Arg1,Arg2) const;  
};

template <class T,class Ret,class Arg1,class Arg2,class Arg3> struct memfun_traits<Ret (T::*)(Arg1,Arg2,Arg3)>
{
  typedef Ret (T::*function)(Arg1,Arg2,Arg3);
  typedef Ret (T::*const_function)(Arg1,Arg2,Arg3) const;  
};

template <class T,class Ret,class Arg1,class Arg2,class Arg3> struct memfun_traits<Ret (T::*)(Arg1,Arg2,Arg3) const>
{
  typedef Ret (T::*function)(Arg1,Arg2,Arg3);
  typedef Ret (T::*const_function)(Arg1,Arg2,Arg3) const;  
};

template <class T,class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
struct memfun_traits<Ret (T::*)(Arg1,Arg2,Arg3,Arg4)>
{
  typedef Ret (T::*function)(Arg1,Arg2,Arg3,Arg4);
  typedef Ret (T::*const_function)(Arg1,Arg2,Arg3,Arg4) const;  
};

template <class T,class Ret,class Arg1,class Arg2,class Arg3,class Arg4>
struct memfun_traits<Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const>
{
  typedef Ret (T::*function)(Arg1,Arg2,Arg3,Arg4);
  typedef Ret (T::*const_function)(Arg1,Arg2,Arg3,Arg4) const;  
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер вызова одноместной функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Ret,class Fn>
class unary_recall
{
  public:
    typedef Arg  argument_type;
    typedef Ret  result_type;
    typedef Fn   function;

    unary_recall (function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () (argument_type) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение адаптеров
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const unary_recall&) const;
    bool operator != (const unary_recall&) const;

  private:
    function fn;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер вызова двуместной функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2,class Ret,class Fn>
class binary_recall
{
  public:
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Ret  result_type;
    typedef Fn   function;

    binary_recall (function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () (first_argument_type,second_argument_type) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение адаптеров
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const binary_recall&) const;
    bool operator != (const binary_recall&) const;

  private:
    function fn;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер вызова трёхместной функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2,class Arg3,class Ret,class Fn>
class ternary_recall
{
  public:
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Arg3 third_argument_type;
    typedef Ret  result_type;
    typedef Fn   function;

    ternary_recall (function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () (first_argument_type,second_argument_type,third_argument_type) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение адаптеров
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const ternary_recall&) const;
    bool operator != (const ternary_recall&) const;

  private:
    function fn;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер вызова четырёхместной функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret,class Fn>
class quaternary_recall
{
  public:
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Arg3 third_argument_type;
    typedef Arg4 fourth_argument_type;
    typedef Ret  result_type;
    typedef Fn   function;

    quaternary_recall (function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () (first_argument_type,second_argument_type,third_argument_type,fourth_argument_type) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение адаптеров
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const quaternary_recall&) const;
    bool operator != (const quaternary_recall&) const;

  private:
    function fn;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер вызова пятиместной функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret,class Fn>
class pentanary_recall
{
  public:
    typedef Arg1  first_argument_type;
    typedef Arg2  second_argument_type;
    typedef Arg3  third_argument_type;
    typedef Arg4  fourth_argument_type;
    typedef Arg5  fifth_argument_type;
    typedef Ret   result_type;
    typedef Fn    function;

    pentanary_recall (function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () (first_argument_type,second_argument_type,third_argument_type,
                             fourth_argument_type,fifth_argument_type) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение адаптеров
/////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const pentanary_recall&) const;
    bool operator != (const pentanary_recall&) const;

  private:
    function fn;
};

/*
    Адаптеры вызовов указателя на функцию
*/

//Адаптер вызова одноместного указателя на функции
template <class Arg,class Ret> 
struct pointer_to_unary_function: public unary_recall<Arg,Ret,Ret (*)(Arg)>
{
  pointer_to_unary_function (Ret (*)(Arg));
};

//Адаптер вызова двуместного указателя на функцию
template <class Arg1,class Arg2,class Ret> 
struct pointer_to_binary_function: public binary_recall<Arg1,Arg2,Ret,Ret (*)(Arg1,Arg2)>
{
  pointer_to_binary_function (Ret (*)(Arg1,Arg2));
};

//Адаптер вызова трёхместного указателя на функцию
template <class Arg1,class Arg2,class Arg3,class Ret> 
struct pointer_to_ternary_function: public ternary_recall<Arg1,Arg2,Arg3,Ret,Ret (*)(Arg1,Arg2,Arg3)>
{
  pointer_to_ternary_function (Ret (*)(Arg1,Arg2,Arg3));
};

//Адаптер вызова четырёхместного указателя на функцию
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret> 
struct pointer_to_quaternary_function: public quaternary_recall<Arg1,Arg2,Arg3,Arg4,Ret,Ret (*)(Arg1,Arg2,Arg3,Arg4)>
{
  pointer_to_quaternary_function (Ret (*)(Arg1,Arg2,Arg3,Arg4));  
};

//Адаптер вызова пятиместного указателя на функцию
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret> 
struct pointer_to_pentanary_function: public pentanary_recall<Arg1,Arg2,Arg3,Arg4,Arg5,Ret,Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5)>
{
  pointer_to_pentanary_function (Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5));
};

/*
    Адаптеры вызовов нестатических методов класса
*/

//Адаптер вызова функции-члена класса (arg->*fn)()
template <class Ret,class T> 
struct mem_fun_t: public unary_recall<T*,Ret,Ret (T::*)()>
{
  mem_fun_t (Ret (T::*)());
};

//Адаптер вызова функции-члена класса (arg->*fn)() const
template <class Ret,class T>
struct const_mem_fun_t: public unary_recall<const T*,Ret,Ret (T::*)() const>
{
  const_mem_fun_t (Ret (T::*)() const);
};

//Адаптер вызова функции-члена класса (arg.*fn)()
template <class Ret,class T> 
struct mem_fun_ref_t: public unary_recall<T&,Ret,Ret (T::*)()>
{
  mem_fun_ref_t (Ret (T::*)());
};

//Адаптер вызова функции-члена класса (arg.*fn)() const
template <class Ret,class T> 
struct const_mem_fun_ref_t: public unary_recall<const T&,Ret,Ret (T::*)() const>
{
  const_mem_fun_ref_t (Ret (T::*)() const);
};

//Адаптер вызова одноместной функции-члена класса (arg1->*fn)(arg2)
template <class Ret,class T,class Arg>
struct mem_fun1_t: public binary_recall<T*,Arg,Ret,Ret (T::*)(Arg)>
{
  mem_fun1_t (Ret (T::*)(Arg));
};

//Адаптер вызова одноместной функции-члена класса (arg1->*fn)(arg2) const
template <class Ret,class T,class Arg>
struct const_mem_fun1_t: public binary_recall<const T*,Arg,Ret,Ret (T::*)(Arg) const>
{
  const_mem_fun1_t (Ret (T::*)(Arg) const);
};

//Адаптер вызова одноместной функции-члена класса (arg1.*fn)(arg2)
template <class Ret,class T,class Arg>
struct mem_fun1_ref_t: public binary_recall<T&,Arg,Ret,Ret (T::*)(Arg)>
{
  mem_fun1_ref_t (Ret (T::*)(Arg));
};

//Адаптер вызова одноместной функции-члена класса (arg1.*fn)(arg2) const
template <class Ret,class T,class Arg>
struct const_mem_fun1_ref_t: public binary_recall<const T&,Arg,Ret,Ret (T::*)(Arg) const>
{
  const_mem_fun1_ref_t (Ret (T::*)(Arg) const);
};

//Адаптер вызова двуместной-функции члена класса (arg1->*fn)(arg2,arg3)
template <class Ret,class T,class Arg1,class Arg2>
struct mem_fun2_t: public ternary_recall<T*,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2)>
{
  mem_fun2_t (Ret (T::*)(Arg1,Arg2));
};

//Адаптер вызова двуместной-функции члена класса (arg1->*fn)(arg2,arg3) const
template <class Ret,class T,class Arg1,class Arg2>
struct const_mem_fun2_t: public ternary_recall<const T*,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2) const>
{
  const_mem_fun2_t (Ret (T::*)(Arg1,Arg2) const);
};

//Адаптер вызова двуместной-функции члена класса (arg1.*fn)(arg2,arg3)
template <class Ret,class T,class Arg1,class Arg2>
struct mem_fun2_ref_t: public ternary_recall<T&,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2)>
{
  mem_fun2_ref_t (Ret (T::*)(Arg1,Arg2));
};

//Адаптер вызова двуместной-функции члена класса (arg1.*fn)(arg2,arg3) const
template <class Ret,class T,class Arg1,class Arg2>
struct const_mem_fun2_ref_t: public ternary_recall<const T&,Arg1,Arg2,Ret,Ret (T::*)(Arg1,Arg2) const>
{
  const_mem_fun2_ref_t (Ret (T::*)(Arg1,Arg2) const);
};

//Адаптер вызова трёхместной-функции члена класса (arg1->*fn)(arg2,arg3,arg4)
template <class Ret,class T,class Arg1,class Arg2,class Arg3>
struct mem_fun3_t: public quaternary_recall<T*,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3)>
{
  mem_fun3_t (Ret (T::*)(Arg1,Arg2,Arg3));
};

//Адаптер вызова трёхместной-функции члена класса (arg1->*fn)(arg2,arg3,arg4) const
template <class Ret,class T,class Arg1,class Arg2,class Arg3>
struct const_mem_fun3_t: public quaternary_recall<const T*,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3) const>
{
  const_mem_fun3_t (Ret (T::*)(Arg1,Arg2,Arg3) const);
};

//Адаптер вызова трёхместной-функции члена класса (arg1.*fn)(arg2,arg3,arg4)
template <class Ret,class T,class Arg1,class Arg2,class Arg3>
struct mem_fun3_ref_t: public quaternary_recall<T&,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3)>
{
  mem_fun3_ref_t (Ret (T::*)(Arg1,Arg2,Arg3));
};

//Адаптер вызова трёхместной-функции члена класса (arg1.*fn)(arg2,arg3,arg4) const
template <class Ret,class T,class Arg1,class Arg2,class Arg3>
struct const_mem_fun3_ref_t: public quaternary_recall<const T&,Arg1,Arg2,Arg3,Ret,Ret (T::*)(Arg1,Arg2,Arg3) const>
{
  const_mem_fun3_ref_t (Ret (T::*)(Arg1,Arg2,Arg3) const);
};

//Адаптер вызова четырёхместной-функции члена класса (arg1->*fn)(arg2,arg3,arg4,arg5)
template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
struct mem_fun4_t: public pentanary_recall<T*,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4)>
{
  mem_fun4_t (Ret (T::*)(Arg1,Arg2,Arg3,Arg4));
};

//Адаптер вызова четырёхместной-функции члена класса (arg1->*fn)(arg2,arg3,arg4,arg5) const
template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
struct const_mem_fun4_t: public pentanary_recall<const T*,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const>
{
  const_mem_fun4_t (Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const);
};

//Адаптер вызова четырёхместной-функции члена класса (arg1.*fn)(arg2,arg3,arg4,arg5)
template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
struct mem_fun4_ref_t: public pentanary_recall<T&,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4)>
{
  mem_fun4_ref_t (Ret (T::*)(Arg1,Arg2,Arg3,Arg4));
};

//Адаптер вызова четырёхместной-функции члена класса (arg1.*fn)(arg2,arg3,arg4,arg5) const
template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
struct const_mem_fun4_ref_t: public pentanary_recall<const T&,Arg1,Arg2,Arg3,Arg4,Ret,Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const>
{
  const_mem_fun4_ref_t (Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объектов-адаптеров
///   ptr_fun     - вызов статической функции
///   mem_fun     - вызов функции-члена класса
///   mem_fun_ref - вызов функции-члена класса по ссылке
/////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Arg,class Ret>
pointer_to_unary_function<Arg,Ret> ptr_fun (Ret (*)(Arg));

template <class Arg1,class Arg2,class Ret>
pointer_to_binary_function<Arg1,Arg2,Ret> ptr_fun (Ret (*)(Arg1,Arg2));

template <class Arg1,class Arg2,class Arg3,class Ret>
pointer_to_ternary_function<Arg1,Arg2,Arg3,Ret> ptr_fun (Ret (*)(Arg1,Arg2,Arg3));

template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
pointer_to_quaternary_function<Arg1,Arg2,Arg3,Arg4,Ret> ptr_fun (Ret (*)(Arg1,Arg2,Arg3,Arg4));

template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
pointer_to_pentanary_function<Arg1,Arg2,Arg3,Arg4,Arg5,Ret> ptr_fun (Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5));

template <class Ret,class T>
mem_fun_t<Ret,T> mem_fun (Ret (T::*)());

template <class Ret,class T>
const_mem_fun_t<Ret,T> mem_fun (Ret (T::*)() const);

template <class Ret,class T,class Arg>
mem_fun1_t<Ret,T,Arg> mem_fun (Ret (T::*)(Arg));

template <class Ret,class T,class Arg>
const_mem_fun1_t<Ret,T,Arg> mem_fun (Ret (T::*)(Arg) const);

template <class Ret,class T,class Arg1,class Arg2>
mem_fun2_t<Ret,T,Arg1,Arg2> mem_fun (Ret (T::*)(Arg1,Arg2));

template <class Ret,class T,class Arg1,class Arg2>
const_mem_fun2_t<Ret,T,Arg1,Arg2> mem_fun (Ret (T::*)(Arg1,Arg2) const);

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
mem_fun3_t<Ret,T,Arg1,Arg2,Arg3> mem_fun (Ret (T::*)(Arg1,Arg2,Arg3));

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
const_mem_fun3_t<Ret,T,Arg1,Arg2,Arg3> mem_fun (Ret (T::*)(Arg1,Arg2,Arg3) const);

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun (Ret (T::*)(Arg1,Arg2,Arg3,Arg4));

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
const_mem_fun4_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun (Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const);

template <class Ret,class T>
mem_fun_ref_t<Ret,T> mem_fun_ref (Ret (T::*)());

template <class Ret,class T>
const_mem_fun_ref_t<Ret,T> mem_fun_ref (Ret (T::*)() const);

template <class Ret,class T,class Arg>
mem_fun1_ref_t<Ret,T,Arg> mem_fun_ref (Ret (T::*)(Arg));

template <class Ret,class T,class Arg>
const_mem_fun1_ref_t<Ret,T,Arg> mem_fun_ref (Ret (T::*)(Arg) const);

template <class Ret,class T,class Arg1,class Arg2>
mem_fun2_ref_t<Ret,T,Arg1,Arg2> mem_fun_ref (Ret (T::*)(Arg1,Arg2));

template <class Ret,class T,class Arg1,class Arg2>
const_mem_fun2_ref_t<Ret,T,Arg1,Arg2> mem_fun_ref (Ret (T::*)(Arg1,Arg2) const);

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3> mem_fun_ref (Ret (T::*)(Arg1,Arg2,Arg3));

template <class Ret,class T,class Arg1,class Arg2,class Arg3>
const_mem_fun3_ref_t<Ret,T,Arg1,Arg2,Arg3> mem_fun_ref (Ret (T::*)(Arg1,Arg2,Arg3) const);

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun_ref (Ret (T::*)(Arg1,Arg2,Arg3,Arg4));

template <class Ret,class T,class Arg1,class Arg2,class Arg3,class Arg4>
const_mem_fun4_ref_t<Ret,T,Arg1,Arg2,Arg3,Arg4> mem_fun_ref (Ret (T::*)(Arg1,Arg2,Arg3,Arg4) const);

#include <stl/detail/adaptors.inl>

}

#endif
