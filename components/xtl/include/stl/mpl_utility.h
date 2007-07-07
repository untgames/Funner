/*
    ”тилиты дл€ метапрограммировани€
*/

#ifndef XTL_MPL_UTILITY_HEADER
#define XTL_MPL_UTILITY_HEADER

#include <stddef.h>

namespace xtl
{

namespace mpl
{

/*
    ”правл€ющие шаблоны
*/

template <bool condition, class TrueType, class FalseType> struct select;              //выбор из двух типов по условию condition
template <bool condition>                                  struct compile_time_assert; //аналог assert во врем€ компил€ции

#include <stl/detail/mpl/utility.inl>

}

}

#endif
