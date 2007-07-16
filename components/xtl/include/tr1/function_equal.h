#ifndef MYTR1_FUNCTION_EQUAL_HEADER
#define MYTR1_FUNCTION_EQUAL_HEADER

namespace tr1
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка равенства функциональных объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Fn1, class Fn2>
bool function_equal (const Fn1&, const Fn2&);

#include <tr1/detail/unwrap.inl>
#include <tr1/detail/function_equal.inl>

}

#endif
