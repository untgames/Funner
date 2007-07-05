#include <stl/functional>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of logicnot_test:\n");
  
  bool input [7] = {1,0,0,1,1,1,1};
 
  printf ("count = %d\n",count_if (input,input+7,logical_not<bool> ()));

  return 0;
}
