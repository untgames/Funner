#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of plus_test:\n");
  
  int input1 [4] = {1,6,11,8}, input2 [4] = {1,5,2,3};

  printf ("total = %d\n",inner_product (input1,input1+4,input2,0,plus<int>(),multiplies<int>()));

  return 0;
}
