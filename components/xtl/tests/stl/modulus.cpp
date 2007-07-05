#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of modulus_test:\n");
  
  int input1 [4] = {6,8,10,2}, input2 [4] = {4,2,11,3}, output [4];
  
  transform ((int*)input1,(int*)input1+4,(int*)input2,(int*)output,modulus<int> ());
  
  for (int i=0;i<4;i++)
    printf ("%d\n",output [i]);
    
  return 0;
}
