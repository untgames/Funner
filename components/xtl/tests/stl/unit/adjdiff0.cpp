#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of adjdiff0_test:\n");
  
  int numbers [5] = {1,2,4,8,16}, difference [5];
  
  adjacent_difference (numbers,numbers+5,difference);
  
  int i;
  
  for (i=0;i<5;i++) printf ("%d ",numbers [i]);
  printf ("\n");
  
  for (i=0;i<5;i++) printf ("%d ",difference [i]);
  printf ("\n");  
  
  return 0;
}
