#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of parsrtc0_test:\n");
  
  int numbers [6] = {5,2,4,3,1,6}, result [3];
  
  partial_sort_copy ((int*)numbers,(int*)numbers+6,(int*)result,(int*)result+3);
  
  for (int i=0;i<3;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");  

  return 0;
}
