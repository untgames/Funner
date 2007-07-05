#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of rotcopy0_test:\n");
  
  int numbers [6] = {0,1,2,3,4,5}, result [6];

  rotate_copy ((int*)numbers,(int*)numbers+3,(int*)numbers+6,(int*)result);
  
  for (int i=0;i<6;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");
    
  return 0;
}
