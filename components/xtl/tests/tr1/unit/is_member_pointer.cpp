#include "test.h"

int main ()
{
  printf ("Results of is_member_pointer_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_member_pointer<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<f2>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<f3>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<void*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<mf4>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_pointer<foo0_t>::value, false);

  return 0;
}
