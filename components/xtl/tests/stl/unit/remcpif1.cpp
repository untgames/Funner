#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of remcpif1_test:\n");

  int numbers [6] = {1,2,3,1,2,3}, result [6] = {0,0,0,0,0,0};

  remove_copy_if ((int*)numbers,(int*)numbers+6,(int*)result,odd);
  
  for (int i=0;i<6;i++)
    printf ("%d ",result [i]);

  printf ("\n");  
    
  return 0;
}
