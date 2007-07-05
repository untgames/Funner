#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of divides_test:\n");
  
  int input [3] = {2,3,4};
  
  printf ("result = %d\n",accumulate (input,input+3,48,divides<int>()));  

  return 0;
}
