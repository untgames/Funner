#include "test.h"

int main ()
{
  printf ("Results of is_reference_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_reference<int&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<const int&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<volatile int &>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<const volatile int &>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<r_type>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<cr_type>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<UDT&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<const UDT&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<volatile UDT&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<const volatile UDT&>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<int (&)(int)>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<int (&)[2]>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_reference<int [2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<const int [2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<volatile int [2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<const volatile int [2]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<bool>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_reference<incomplete_type>::value, false);

  return 0;
}

