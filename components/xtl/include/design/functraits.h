/*
    Этот код был сгенерирован автоматически. Не исправляйте его вручную
*/

#ifndef DESIGN_FUNCTION_TRAITS_HEADER
#define DESIGN_FUNCTION_TRAITS_HEADER

#include <mpl/type_list.h>

namespace mpl
{

/*
    Таблицы конфигурации аргументов функции и типа возвращаего значения
*/

//тип используемый в качестве индикатора отсутствия аргумента, аналогичен void
struct void_argument {};

//обёртка указывающая на наличие многоточия в списке аргументов функции
template <class Traits>
struct eclipsis_function_traits: public Traits
{
  enum { has_eclipsis = true };
};

//таблица конфигурации для сигнатур на функции
template <class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
class signature_arguments_traits
{
  private:
    typedef typename type_node<Arg1,type_node<Arg2,type_node<Arg3,type_node<Arg4,type_node<Arg5,type_node<Arg6,type_node<Arg7,type_node<Arg8,type_node<Arg9,null_type> > > > > > > > > arguments_list;

  public:
    enum {
      is_ptrfun    = false,
      is_memfun    = false,
      has_eclipsis = false
    };

    typedef Ret result_type;

    template <size_t number> struct argument {
      typedef typename mpl::at<arguments_list,number-1>::type type;
    };
};

//таблица конфигурации для указателей на функции
template <class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
struct ptrfun_arguments_traits: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  enum { is_ptrfun = true };
};

//таблица конфигурации для указателей на функции-члены
template <class T,class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
struct memfun_arguments_traits: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  typedef T object_type;

  enum { is_memfun = true };
};

//таблица конфигурации функционального объекта
template <class Fn>
struct function_traits
{
  enum {
    is_memfun    = false,
    is_ptrfun    = false,
    has_eclipsis = Fn::has_eclipsis
  };

  typedef typename Fn::result_type result_type;

  template <size_t number> struct argument {
    typedef typename Fn::template argument<number>::type type;
  };
};

/*
    Специализации для сигнатур функций
*/

//специализация для сигнатуры без аргументов
template <class Ret>
struct function_traits<Ret ()>: public signature_arguments_traits<Ret> { };

//специализация для сигнатуры без аргументов и многоточием
template <class Ret>
struct function_traits<Ret (...)>: public eclipsis_function_traits<signature_arguments_traits<Ret> > { };

//специализация для сигнатуры с 1-м аргументом
template <class Arg1,class Ret>
struct function_traits<Ret (Arg1)>: public signature_arguments_traits<Ret,Arg1> { };

//специализация для сигнатуры с 1-м аргументом и многоточием
template <class Arg1,class Ret>
struct function_traits<Ret (Arg1,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1> > { };

//специализация для сигнатуры с 2-мя аргументами
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (Arg1,Arg2)>: public signature_arguments_traits<Ret,Arg1,Arg2> { };

//специализация для сигнатуры с 2-мя аргументами и многоточием
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (Arg1,Arg2,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2> > { };

//специализация для сигнатуры с 3-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3> { };

//специализация для сигнатуры с 3-мя аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3> > { };

//специализация для сигнатуры с 4-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для сигнатуры с 4-мя аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для сигнатуры с 5-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для сигнатуры с 5-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для сигнатуры с 6-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для сигнатуры с 6-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для сигнатуры с 7-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для сигнатуры с 7-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для сигнатуры с 8-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для сигнатуры с 8-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для сигнатуры с 9-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для сигнатуры с 9-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    Специализации для указателей на функцию
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct function_traits<Ret (*)()>: public ptrfun_arguments_traits<Ret>  { };

//специализация для указателя на функцию без аргументов и многоточием
template <class Ret>
struct function_traits<Ret (*)(...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret> > { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1,class Ret>
struct function_traits<Ret (*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//специализация для указателя на функцию с 1-м аргументом и многоточием
template <class Arg1,class Ret>
struct function_traits<Ret (*)(Arg1,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1> > { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//специализация для указателя на функцию с 2-мя аргументами и многоточием
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2> > { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//специализация для указателя на функцию с 3-мя аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3> > { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//специализация для указателя на функцию с 4-мя аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//специализация для указателя на функцию с 5-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//специализация для указателя на функцию с 6-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//специализация для указателя на функцию с 7-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//специализация для указателя на функцию с 8-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

//специализация для указателя на функцию с 9-ю аргументами и многоточием
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    Специализации для указателей на функцию-член класса
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//специализация для указателя на функцию-член класса T без аргументов и многоточием
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret> > { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов и многоточием
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret> > { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов и многоточием
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret> > { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов и многоточием
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret> > { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом и многоточием
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1> > { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом и многоточием
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1> > { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом и многоточием
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1> > { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом и многоточием
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1> > { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2> > { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2> > { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> > { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> > { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> > { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> > { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> > { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> > { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами и многоточием
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    Специализации для компилятора MSVC
*/

#ifdef _MSC_VER

/*
    Специализации для указателей на функцию с модификатором __fastcall
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct function_traits<Ret (__fastcall*)()>: public ptrfun_arguments_traits<Ret>  { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

/*
    Специализации для указателей на функцию-член класса с модификатором __fastcall
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

/*
    Специализации для указателей на функцию с модификатором __stdcall
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct function_traits<Ret (__stdcall*)()>: public ptrfun_arguments_traits<Ret>  { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

/*
    Специализации для указателей на функцию-член класса с модификатором __stdcall
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

#endif //_MSC_VER

}

namespace design
{
  namespace type_manip = mpl;
}

#endif //DESIGN_FUNCTION_TRAITS_HEADER
