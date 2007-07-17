#ifndef XTL_RESULT_OF_HEADER
#define XTL_RESULT_OF_HEADER

#include <xtl/functional_traits.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение типа возвращаемого значения функции
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FunctionCallType> struct result_of;

#include <xtl/detail/result_of.inl>

}

namespace tr1
{

using xtl::result_of;

}

#endif
