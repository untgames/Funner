#include "test.h"

typedef const double (UDT::*mp2) ;

int main ()
{
  printf ("Results of is_member_obj_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<f2>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<f3>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<void*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mf1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mf2>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mf3>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mf4>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<cmf>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mp>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<mp2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_member_object_pointer<foo0_t>::value, false);

  return 0;
}
