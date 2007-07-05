#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of iterswp0_test:\n");
  
  int numbers [6] = {0,1,2,3,4,5};

  iter_swap (numbers,numbers+3);
    
  for (int i=0;i<6;i++) 
    printf ("%d ",numbers [i]);

  printf ("\n");  
    
  return 0;
}
