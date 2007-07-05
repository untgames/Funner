#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of minus_test:\n");
  
  int input1 [4] = {1,5,7,8}, input2 [4] = {1,4,8,3}, output [4];
  
  transform ((int*)input1,(int*)input1+4,(int*)input2,(int*)output,minus<int> ());
  
  for (int i=0;i<4;i++)
    printf ("%d\n",output [i]);
    
  return 0;
}
