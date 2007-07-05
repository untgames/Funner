#include "test.h"

int main ()
{
  printf ("Results of has_nothrow_consr_test:\n");

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int[3][2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<int[2][4][5][6][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<nothrow_assign_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<nothrow_copy_UDT>::value, false);

  CHECK_INTEGRAL_CONSTANT (has_nothrow_constructor<test_abc1>::value, false);

  return 0;
}
