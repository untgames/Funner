#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of nextprm0_test:\n");
  
  int v1 [3] = {0,1,2};

  next_permutation (v1,v1+3);
  
  for (int i=0;i<3;i++)  printf ("%d ",v1 [i]);
  printf ("\n");
    
  return 0;
}
