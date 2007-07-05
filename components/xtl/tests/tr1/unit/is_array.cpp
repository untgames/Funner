#include "test.h"

struct convertible_to_pointer
{
  operator char* () const;
};

int main ()
{
  printf ("Results of is_array_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_array<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<const int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<const volatile int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<int*const>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<const int*volatile>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<const volatile int*const>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_array<const int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_array<const volatile int[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_array<int[2][3]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_array<UDT[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_array<int(&)[2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<f1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<convertible_to_pointer>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_array<incomplete_type>::value, false);
  
  return 0;
}
