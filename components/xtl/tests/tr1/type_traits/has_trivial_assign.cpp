#include "test.h"

int main ()
{
  printf ("Results of has_trivial_assign_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<enum_UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<trivial_except_assign>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<wrap<trivial_except_assign> >::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_assign<test_abc1>::value, false);

  return 0;
}
