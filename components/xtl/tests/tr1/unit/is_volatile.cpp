#include "test.h"

int main ()
{
  printf ("Results of is_volatile_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_volatile<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile void>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile test_abc1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile const UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<incomplete_type>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<const int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<const volatile int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<const volatile int[]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<const int[]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_volatile<volatile int[]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_volatile<int[]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_const<const volatile int[3][4][5]>::value, true);  
  
  return 0;
}
