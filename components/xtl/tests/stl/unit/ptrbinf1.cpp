#include <stl/algorithm>
#include <stdio.h>

int sum (int x,int y)
{
  return x + y;
}

using namespace stl;

int main ()
{
  printf ("Results of ptrbinf1_test:\n");

  int input1 [4] = {7,2,3,5}, input2 [4] = {1,5,5,8}, output [4];
  
  transform (input1,input1+4,input2,output,pointer_to_binary_function<int,int,int> (sum));
  
  for (int i=0;i<4;i++)
    printf ("%d\n",output [i]);
    
  return 0;
}
