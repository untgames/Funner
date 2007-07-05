#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of alg2_test:\n");
  
  int i [] = {1,4,2,8,2,2};

  printf ("Count of 2s = %d\n",(int)count (i,i+6,2));
  
  return 0;
}
