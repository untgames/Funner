#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of replace0_test:\n");
  
  int numbers [6] = {0,1,2,0,1,2};

  replace (numbers,numbers+6,2,42);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);
    
  printf ("\n");
    
  return 0;
}
