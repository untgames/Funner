#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of rotate0_test:\n");
  
  int numbers [6] = {0,1,2,3,4,5};

  rotate ((int*)numbers,numbers+4,numbers+6);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);
    
  printf ("\n");
    
  return 0;
}
