#include "test.h"

int main ()
{
  printf ("Results of is_object_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_object<int>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_object<UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_object<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_object<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_object<foo4_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_object<test_abc1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_object<incomplete_type>::value, true);

  return 0;
}
