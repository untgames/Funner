#include "test.h"

int main ()
{
  printf ("Results of is_arithmetic_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_arithmetic<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<const float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<float[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_arithmetic<incomplete_type>::value, false);

  return 0;
}
