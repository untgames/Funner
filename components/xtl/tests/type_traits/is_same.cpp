#include "test.h"

int main ()
{
  printf ("Results of is_same_test:\n");

  CHECK_INTEGRAL_CONSTANT ((is_same<int, int>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_same<int, const int>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int, int&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<const int, int&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int, const int&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int*, const int*>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int*, int*const>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int, int[2]>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int*, int[2]>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_same<int[4], int[2]>::value), false);

  return 0;
}
