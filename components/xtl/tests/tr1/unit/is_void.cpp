#include "test.h"

int main ()
{
  printf ("Results of is_void_test:\n");
  
  CHECK_INTEGRAL_CONSTANT (is_void<void>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_void<void const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_void<void volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_void<void const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_void<void*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<foo1_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<foo2_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<foo3_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<foo4_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_void<incomplete_type>::value, false);

  return 0;
}
