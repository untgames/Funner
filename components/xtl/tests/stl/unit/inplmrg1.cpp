#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of inplmrg1_test:\n");
  
  int numbers [6] = {1,10,42,3,16,32}, i;

  for (i=0;i<6;i++) printf ("%d ",numbers [i]);
  printf ("\n");
  
  inplace_merge (numbers,numbers+3,numbers+6);

  for (i=0;i<6;i++) printf ("%d ",numbers [i]);
  printf ("\n");  

  return 0;
}
