#include "test.h"

int main ()
{
  printf ("Results of is_floating_point_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_floating_point<float>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_floating_point<double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_floating_point<long double>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<long double const>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<long double volatile>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<long double const volatile>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_floating_point<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<empty_UDT>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<const float&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<float[2]>::value, false);

  CHECK_INTEGRAL_CONSTANT (is_floating_point<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<foo0_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<foo1_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<foo2_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<foo3_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<foo4_t>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_floating_point<incomplete_type>::value, false);

  return 0;
}
