#include "test.h"

int main ()
{
  printf ("Results of rank_test:\n");

  CHECK_INTEGRAL_CONSTANT (rank<int>::value, 0);
  CHECK_INTEGRAL_CONSTANT (rank<int[]>::value, 1);
  CHECK_INTEGRAL_CONSTANT (rank<int[][10]>::value, 2);
  CHECK_INTEGRAL_CONSTANT (rank<int[5][10]>::value, 2);
  CHECK_INTEGRAL_CONSTANT (rank<int[5][10][40]>::value, 3);
  CHECK_INTEGRAL_CONSTANT (rank<int (&)[5][10]>::value, 0);
  CHECK_INTEGRAL_CONSTANT (rank<int (*)[5][10]>::value, 0);
 
  return 0;
}
