#include "test.h"

int main ()
{
  printf ("Results of is_fundamental_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_fundamental<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<void const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<void volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<void const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_fundamental<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<const float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<float[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<incomplete_type>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_fundamental<foo0_t>::value, false);

  return 0;
}
