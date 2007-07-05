#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of partsrt0_test:\n");
  
  int numbers [6] = {5,2,4,3,1,6};

  partial_sort (numbers,numbers+3,numbers+6);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);

  printf ("\n");  
    
  return 0;
}
