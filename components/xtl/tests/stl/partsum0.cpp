#include <stl/algorithm>
#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of partsum0_test:\n");
  
  int numbers [6] = {1,2,3,4,5,6}, result [6];

  partial_sum (numbers,numbers+6,result);
  
  for (int i=0;i<6;i++) printf ("%d ",result [i]);
  
  printf ("\n");
  
  return 0;
}
