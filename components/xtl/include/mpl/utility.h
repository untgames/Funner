/*
    ”тилиты дл€ метапрограммировани€
*/

#ifndef MPL_UTILITY_HEADER
#define MPL_UTILITY_HEADER

#include <stddef.h>

namespace mpl
{

/*
    “ипы-селекторы
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///“ипы позвол€ющие делать выбор в compile-time по размеру структуры
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef char                       no_type;
typedef struct { char dummy [2]; } yes_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///¬спомогательный шаблонный класс описывающий целочисленную константу
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, T v> struct integral_constant
{
  static  const T                 value = v;
  typedef T                       value_type;
  typedef integral_constant<T, v> type; 
};

//вспомогательные классы дл€ наиболее распространЄнных целочисленных костант
template <bool value> struct bool_constant:    public integral_constant<bool, value> {};
template <int value>  struct integer_constant: public integral_constant<int, value> {};

typedef bool_constant<true>  true_type;
typedef bool_constant<false> false_type;

/*
    ”правл€ющие шаблоны
*/

template <bool condition, class TrueType, class FalseType> struct select;              //выбор из двух типов по условию condition
template <bool condition>                                  struct compile_time_assert; //аналог assert во врем€ компил€ции

#include <mpl/detail/utility.inl>

}

#endif
