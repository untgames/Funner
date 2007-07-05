#include "test.h"

int main ()
{
  printf ("Results of has_nothrow_copy_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<long double const>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<nothrow_assign_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<nothrow_construct_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_copy<test_abc1>::value, false);
  
  return 0;
}
