#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of copyb0_test:\n");
  
  int numbers[5] = {1,2,3,4,5}, result[5];
  
  copy_backward (numbers,numbers+5,result+5);
  
  int i;
  
  for (i=0;i<5;i++) printf ("%d ",numbers [i]);
  printf ("\n");

  for (i=0;i<5;i++) printf ("%d ",result [i]);
  printf ("\n");
  
  return 0;
}
