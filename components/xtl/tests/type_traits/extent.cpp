#include "test.h"

int main ()
{
  printf ("Results of extent_test:\n");

  CHECK_INTEGRAL_CONSTANT (extent<int>::value, 0);
  CHECK_INTEGRAL_CONSTANT (extent<int[]>::value, 0);
  CHECK_INTEGRAL_CONSTANT (extent<int[][10]>::value, 0);
  CHECK_INTEGRAL_CONSTANT (extent<int[5][10]>::value, 5);
  CHECK_INTEGRAL_CONSTANT (extent<int[5][10][40]>::value, 5);
  CHECK_INTEGRAL_CONSTANT (extent<int (&)[5][10]>::value, 0);
  CHECK_INTEGRAL_CONSTANT (extent<int (*)[5][10]>::value, 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int,1>::value), 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int[],1>::value), 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int[][10],1>::value), 10);
  CHECK_INTEGRAL_CONSTANT ((extent<int[5][10],1>::value), 10);
  CHECK_INTEGRAL_CONSTANT ((extent<int[5][10][40],1>::value), 10);
  CHECK_INTEGRAL_CONSTANT ((extent<int (&)[5][10],1>::value), 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int (*)[5][10],1>::value), 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int[5][10],2>::value), 0);
  CHECK_INTEGRAL_CONSTANT ((extent<int[5][10][40],2>::value), 40);
  CHECK_INTEGRAL_CONSTANT ((extent<int[5][10][40],3>::value), 0);
  
  return 0;
}
