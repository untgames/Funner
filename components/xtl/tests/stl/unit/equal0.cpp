#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of equal0_test:\n");
  
  int numbers1 [5] = {1,2,3,4,5}, numbers2 [5] = {1,2,4,8,16}, numbers3 [2] = {1,2};

  if (equal (numbers1,numbers1+5,numbers2)) printf ("numbers1 is equal to numbers2\n");
  else                                      printf ("numbers1 is not equal to numbers2\n");
  if (equal (numbers3,numbers3+2,numbers1)) printf ("numbers3 is equal to numbers1\n");
  else                                      printf ("numbers3 is not equal to numbers1\n");
  
  return 0;
}
