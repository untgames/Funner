/*
    ”тилиты дл€ метапрограммировани€
*/

#ifndef MPL_UTILITY_HEADER
#define MPL_UTILITY_HEADER

#include <stddef.h>

namespace mpl
{

/*
    ”правл€ющие шаблоны
*/

template <bool condition, class TrueType, class FalseType> struct select;              //выбор из двух типов по условию condition
template <bool condition>                                  struct compile_time_assert; //аналог assert во врем€ компил€ции

#include <mpl/detail/utility.inl>

}

#endif
