#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

inline int sum (int x,int y) { return x + y; }

int main ()
{
  printf ("Results of ptrbinf2_test:\n");

  int input1 [4] = {7,2,3,5}, input2 [4] = {1,5,5,8}, output [4];
  
  transform ((int*)input1,(int*)input1+4,(int*)input2,(int*)output,ptr_fun (sum));
  
  for (int i=0;i<4;i++)
    printf ("%d\n",output [i]);
    
  return 0;
}
