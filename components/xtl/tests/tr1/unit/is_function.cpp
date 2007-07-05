#include "test.h"
#include <typeinfo>

int main ()
{
  typedef void foo0_t();
  typedef void foo1_t(int);
  typedef void foo2_t(int&, double);
  typedef void foo3_t(int&, bool, int, int);
  typedef void foo4_t(int, bool, int*, int[], int, int, int, int, int);
  
  printf ("Results of is_function_test:\n");

  CHECK_INTEGRAL_CONSTANT (is_function<foo0_t>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_function<foo1_t>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_function<foo2_t>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_function<foo3_t>::value, true);
  CHECK_INTEGRAL_CONSTANT (is_function<foo4_t>::value, true);

  CHECK_INTEGRAL_CONSTANT (is_function<void>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<int>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<int&>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<int*>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<int[]>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<test_abc1>::value, false);
  CHECK_INTEGRAL_CONSTANT (is_function<int (*)(int)>::value, false);
  
  return 0;
}
