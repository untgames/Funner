#include <stl/algorithm>
#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of times_test:\n");
  
  int input [4] = {1,5,7,2};

  printf ("total = %d\n",accumulate (input,input+4,1,multiplies<int> ()));

  return 0;
}
