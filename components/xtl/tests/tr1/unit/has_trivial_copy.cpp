#include "test.h"

int main ()
{
  printf ("Results of has_trivial_copy_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<void>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<trivial_except_copy>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<wrap<trivial_except_copy> >::value, false);

  CHECK_INTEGRAL_CONSTANT (has_trivial_copy<test_abc1>::value, false);

  return 0;
}
