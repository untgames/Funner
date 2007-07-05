#include <stl/functional>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of logicand_test:\n");
  
  int input1 [4] = {1,1,0,1}, input2 [4] = {0,1,0,0}, output [4];
  
  transform ((int*)input1,(int*)input1+4,(int*)input2,(int*)output,logical_and<int> ());
  
  for(int i=0;i<4;i++)
    printf ("%d\n",output [i]);

  return 0;
}
