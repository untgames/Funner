/*
    Этот код был сгенерирован автоматически. Не исправляйте его вручную
*/

#ifndef XTL_FUNCTIONAL_TRAITS_HEADER
#define XTL_FUNCTIONAL_TRAITS_HEADER

#include <cstddef>

namespace xtl
{

//forward declarations
namespace mpl
{

template <class Head, class Tail>       struct type_node;
template <class TypeList, size_t Index> struct at;
                                        struct null_type;

}

namespace detail
{

/*
    Таблица конфигурации функционального объекта
*/

template <class Fn> struct functional_traits
{
  enum {
    is_function  = false,
    is_memfun    = false,
    is_ptrfun    = false,
    has_eclipsis = false
  };
};

/*
    Специализации для различных видов квалификаторов
*/

template <class Fn> struct functional_traits<const Fn>:          public functional_traits<Fn> {};
template <class Fn> struct functional_traits<volatile Fn>:       public functional_traits<Fn> {};
template <class Fn> struct functional_traits<const volatile Fn>: public functional_traits<Fn> {};

/*
    Таблицы конфигурации аргументов функции и типа возвращаемого значения
*/

//тип используемый в качестве индикатора отсутствия аргумента, аналогичен void
struct void_argument {};

//обёртка указывающая на наличие многоточия в списке аргументов функции
template <class Traits>
struct eclipsis_functional_traits: public Traits
{
  enum { has_eclipsis = true };
};

//таблица конфигурации для сигнатур на функции
template <size_t ArgumentsCount, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
class signature_arguments_traits
{
  private:
    typedef typename mpl::type_node<Arg1, mpl::type_node<Arg2, mpl::type_node<Arg3, mpl::type_node<Arg4, mpl::type_node<Arg5, mpl::type_node<Arg6, mpl::type_node<Arg7, mpl::type_node<Arg8, mpl::type_node<Arg9, mpl::null_type> > > > > > > > > argument_list;

  public:
    enum {
      is_function     = true,
      is_ptrfun       = false,
      is_memfun       = false,
      has_eclipsis    = false,
      arguments_count = ArgumentsCount
    };

    typedef Ret result_type;

    template <size_t number> struct argument {
      typedef typename mpl::at<argument_list, number-1>::type type;
    };
};

//таблица конфигурации для указателей на функции
template <size_t ArgumentsCount, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
struct ptrfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  enum { is_ptrfun = true, is_function = false };
};

//таблица конфигурации для указателей на функции-члены
template <size_t ArgumentsCount, class T, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
struct memfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>
{
  typedef T object_type;

  enum { is_memfun = true, is_function = false };
};

/*
    Специализации для сигнатур функций
*/

//специализация для сигнатуры без аргументов
template <class Ret>
struct functional_traits<Ret ()>: public signature_arguments_traits<0, Ret> { };

//специализация для сигнатуры без аргументов и многоточием
template <class Ret>
struct functional_traits<Ret (...)>: public eclipsis_functional_traits<signature_arguments_traits<0, Ret> > { };

//специализация для сигнатуры с 1-м аргументом
template <class Arg1, class Ret>
struct functional_traits<Ret (Arg1)>: public signature_arguments_traits<1, Ret, Arg1> { };

//специализация для сигнатуры с 1-м аргументом и многоточием
template <class Arg1, class Ret>
struct functional_traits<Ret (Arg1, ...)>: public eclipsis_functional_traits<signature_arguments_traits<1, Ret, Arg1> > { };

//специализация для сигнатуры с 2-мя аргументами
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (Arg1, Arg2)>: public signature_arguments_traits<2, Ret, Arg1, Arg2> { };

//специализация для сигнатуры с 2-мя аргументами и многоточием
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (Arg1, Arg2, ...)>: public eclipsis_functional_traits<signature_arguments_traits<2, Ret, Arg1, Arg2> > { };

//специализация для сигнатуры с 3-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3)>: public signature_arguments_traits<3, Ret, Arg1, Arg2, Arg3> { };

//специализация для сигнатуры с 3-мя аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<signature_arguments_traits<3, Ret, Arg1, Arg2, Arg3> > { };

//специализация для сигнатуры с 4-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4)>: public signature_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для сигнатуры с 4-мя аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<signature_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для сигнатуры с 5-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5)>: public signature_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для сигнатуры с 5-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<signature_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для сигнатуры с 6-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public signature_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для сигнатуры с 6-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<signature_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для сигнатуры с 7-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public signature_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для сигнатуры с 7-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<signature_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для сигнатуры с 8-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public signature_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для сигнатуры с 8-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<signature_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для сигнатуры с 9-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public signature_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для сигнатуры с 9-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<signature_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

/*
    Специализации для указателей на функцию-член класса
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//специализация для указателя на функцию-член класса T без аргументов и многоточием
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...)>: public eclipsis_functional_traits<memfun_arguments_traits<0, T, Ret> > { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов и многоточием
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) const>: public eclipsis_functional_traits<memfun_arguments_traits<0, const T, Ret> > { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов и многоточием
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<0, volatile T, Ret> > { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов и многоточием
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<0, const volatile T, Ret> > { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом и многоточием
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<1, T, Ret, Arg1> > { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом и многоточием
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<1, const T, Ret, Arg1> > { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом и многоточием
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<1, volatile T, Ret, Arg1> > { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом и многоточием
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<1, const volatile T, Ret, Arg1> > { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<2, T, Ret, Arg1, Arg2> > { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> > { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> > { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> > { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> > { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> > { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> > { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> > { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами и многоточием
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

#if !defined(_MSC_VER) || defined(_WIN32_WCE)

/*
    Специализации для указателей на функцию
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct functional_traits<Ret (*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//специализация для указателя на функцию без аргументов и многоточием
template <class Ret>
struct functional_traits<Ret (*)(...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<0, Ret> > { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1, class Ret>
struct functional_traits<Ret (*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//специализация для указателя на функцию с 1-м аргументом и многоточием
template <class Arg1, class Ret>
struct functional_traits<Ret (*)(Arg1, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<1, Ret, Arg1> > { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//специализация для указателя на функцию с 2-мя аргументами и многоточием
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<2, Ret, Arg1, Arg2> > { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//специализация для указателя на функцию с 3-мя аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3> > { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//специализация для указателя на функцию с 4-мя аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//специализация для указателя на функцию с 5-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//специализация для указателя на функцию с 6-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//специализация для указателя на функцию с 7-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//специализация для указателя на функцию с 8-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

//специализация для указателя на функцию с 9-ю аргументами и многоточием
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

#else //_MSC_VER

/*
    Специализации для компилятора MSVC
*/

/*
    Специализации для указателей на функцию с модификатором __fastcall
*/

#ifndef __cplusplus_cli

//специализация для указателя на функцию без аргументов
template <class Ret>
struct functional_traits<Ret (__fastcall*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    Специализации для указателей на функцию-член класса с модификатором __fastcall
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

#endif

/*
    Специализации для указателей на функцию с модификатором __stdcall
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct functional_traits<Ret (__stdcall*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    Специализации для указателей на функцию-член класса с модификатором __stdcall
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

/*
    Специализации для указателей на функцию с модификатором __cdecl
*/

//специализация для указателя на функцию без аргументов
template <class Ret>
struct functional_traits<Ret (__cdecl*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//специализация для указателя на функцию с 1-м аргументом
template <class Arg1, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//специализация для указателя на функцию с 2-мя аргументами
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//специализация для указателя на функцию с 3-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//специализация для указателя на функцию с 4-мя аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//специализация для указателя на функцию с 5-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//специализация для указателя на функцию с 6-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//специализация для указателя на функцию с 7-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//специализация для указателя на функцию с 8-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//специализация для указателя на функцию с 9-ю аргументами
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    Специализации для указателей на функцию-член класса с модификатором __cdecl
*/

//специализация для указателя на функцию-член класса T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//специализация для указателя на функцию-член класса const T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//специализация для указателя на функцию-член класса volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//специализация для указателя на функцию-член класса const volatile T без аргументов
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//специализация для указателя на функцию-член класса T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса const volatile T с 1-м аргументом
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//специализация для указателя на функцию-член класса T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса const volatile T с 2-мя аргументами
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//специализация для указателя на функцию-член класса T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса const volatile T с 3-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//специализация для указателя на функцию-член класса T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса const volatile T с 4-мя аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//специализация для указателя на функцию-член класса T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса const volatile T с 5-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//специализация для указателя на функцию-член класса T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса const volatile T с 6-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//специализация для указателя на функцию-член класса T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса const volatile T с 7-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//специализация для указателя на функцию-член класса T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса const volatile T с 8-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//специализация для указателя на функцию-член класса T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//специализация для указателя на функцию-член класса const volatile T с 9-ю аргументами
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

#endif //_MSC_VER

}

using detail::functional_traits;

}

#endif //XTL_FUNCTIONAL_TRAITS_HEADER
