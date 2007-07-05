#include "test.h"

int main ()
{
  printf ("Results of is_enum_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_enum<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_enum<enum_UDT>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_enum<int_convertible>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_enum<int&>::value, false);
//  CHECK_INTEGRAL_CONSTANT (is_enum<noncopyable>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_enum<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_enum<test_abc1>::value, false);

  return 0;
}
