#ifndef XTL_FUNCTION_EQUAL_HEADER
#define XTL_FUNCTION_EQUAL_HEADER

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка равенства функциональных объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Fn1, class Fn2>
bool function_equal (const Fn1&, const Fn2&);

#include <xtl/detail/unwrap.inl>
#include <xtl/detail/function_equal.inl>

}

#endif
