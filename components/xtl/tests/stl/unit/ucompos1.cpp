#include <stl/algorithm>
#include <stdio.h>
#include <math.h>

using namespace stl;

int square_root (int x)
{
  return (int)sqrt ((float)x);
}

int main ()
{
  printf ("Results of ucompos1_test:\n");

  int input [3] = {-1,-4,-16}, output [3];

  transform (input,input+3,output,compose1 (ptr_fun (square_root),negate<int> ()));
  
  for (int i=0;i<3;i++)
    printf ("%d\n",output [i]);

  return 0;
}
