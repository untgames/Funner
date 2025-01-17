#include "test.h"

int main ()
{
  printf ("Results of is_scalar_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_scalar<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<long double const volatile>::value, true);


  CHECK_INTEGRAL_CONSTANT (is_scalar<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_scalar<enum_UDT>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_scalar<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_scalar<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_scalar<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_scalar<test_abc1>::value, false);

  return 0;
}
