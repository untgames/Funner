#include "test.h"

int main ()
{
  printf ("Results of is_member_func_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<f2>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<f3>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<void*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<mf1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<mf2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<mf3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<mf4>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<cmf>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<mp>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<const int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<const int[2] >::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<const int[] >::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_function_pointer<void>::value, false);

  return 0;
}
