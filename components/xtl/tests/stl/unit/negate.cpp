#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of negate_test:\n");
  
  int input [3] = {1,2,3}, output [3];
  
  transform ((int*)input,(int*)input+3,(int*)output,negate<int> ());
  
  for (int i=0;i<3;i++)
    printf ("%d\n",output[i]);
    
  return 0;
}
