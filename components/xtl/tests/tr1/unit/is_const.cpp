#include "test.h"

int main ()
{
  printf ("Results of is_const_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_const<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const void>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const test_abc1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<const UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<const volatile UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<cr_type>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<incomplete_type>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<volatile int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const volatile int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<const volatile int[]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<volatile int[]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const int[]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_const<int[]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const volatile int[3][4][5]>::value, true);

  return 0;
}
