#include "test.h"

int main ()
{
  printf ("Results of is_integral_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_integral<bool>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<bool const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<bool volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<bool const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_integral<signed char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<signed char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<signed char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<signed char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<char>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<char const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<char volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned char const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<char const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<short>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<short const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<short volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned short const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<short const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<int const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<int volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned int const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<int const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<long>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<long const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<long volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<unsigned long const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_integral<long const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_integral<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<float>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<foo1_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<foo2_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<foo3_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<foo4_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_integral<incomplete_type>::value, false);

  return 0;
}
