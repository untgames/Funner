#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of equalto_test:\n");
  int input1 [4] = {1,7,2,2}, input2 [4] = {1,6,2,3}, output [4];
  
  transform ((int*)input1,(int*)input1+4,(int*)input2,(int*)output,equal_to<int>());
  
  for (int i=0;i<4;i++)
    printf ("%d\n",output [i]);

  return 0;
}
