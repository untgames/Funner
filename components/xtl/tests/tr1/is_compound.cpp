#include "test.h"

int main ()
{
  printf ("Results of is_compound_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_compound<UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_compound<void*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_compound<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_compound<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_compound<test_abc1>::value, true);

  return 0;
}
