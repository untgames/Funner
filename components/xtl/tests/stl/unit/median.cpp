#include <stdio.h>
#include <stl/algorithm>

using namespace stl;

int main () 
{
  printf ("Results of median_test:\n");

  int v1 [3] = {0,1,2};

  do    printf ("(%d,%d,%d) %d\n",v1 [0],v1 [1], v1 [2],median (v1 [0],v1 [1],v1 [2]));
  while (next_permutation (v1,v1+3));

  return 0;
}
