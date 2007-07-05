#include "test.h"

int main ()
{
  printf ("Results of has_trivial_constr_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<void>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<trivial_except_construct>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<wrap<trivial_except_construct> >::value, false);

  CHECK_INTEGRAL_CONSTANT (has_trivial_constructor<test_abc1>::value, false);
  
  return 0;
}
