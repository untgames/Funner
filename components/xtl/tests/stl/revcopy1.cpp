#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of revcopy1_test:\n");
  
  int numbers [6] = {0,1,2,3,4,5}, result [6];

  reverse_copy ((int*)numbers,(int*)numbers+6,result);
  
  int i;
  
  for (i=0;i<6;i++) printf ("%d ",numbers [i]);
  printf ("\n");
  
  for (i=0;i<6;i++) printf ("%d ",result [i]);
  printf ("\n");
  
  return 0;
}
