#include "test.h"

int main ()
{
  printf ("Results of has_nothrow_assign_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<long>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<void*>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<nothrow_copy_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<nothrow_construct_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_assign<test_abc1>::value, false);

  return 0;
}
