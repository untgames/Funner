#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of inrprod0_test:\n");
  
  int vector1 [5] = {1,2,3,4,5}, vector2 [5] = {1,2,3,4,5};

  printf ("Inner product = %d\n",inner_product (vector1,vector1+5,vector2,0));

  return 0;
}
