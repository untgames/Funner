#include "test.h"

int main ()
{
  printf ("Results of is_pointer_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_pointer<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<const int*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<volatile int*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<non_pointer*>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int*const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int*volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int*const volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<non_pointer>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int*&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int(&)[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<int[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<char[sizeof(void*)]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<void>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_pointer<f1>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<f2>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<f3>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_pointer<mf1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<mf2>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<mf3>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<mf4>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<test_abc1>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_pointer<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<foo1_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<foo2_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<foo3_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_pointer<foo4_t>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_pointer<test_abc1>::value, false);
  
  return 0;
}
