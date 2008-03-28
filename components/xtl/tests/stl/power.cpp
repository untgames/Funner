#include <stl/numeric>
#include <stdio.h>

using namespace stl;

void test (int value, int degree)
{
  printf ("power(%d,%d)=%d\n", value, degree, power (value, degree));
}

int main ()
{
  printf ("Results of power_test:\n");
  
  test (12, 0);
  test (2, 3);
  test (4, 2);
  test (3, 5);
  test (1, 2);
  test (0, 3);
  test (4, -2);  

  return 0;
}
