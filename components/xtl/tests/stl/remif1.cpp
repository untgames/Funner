#include <stl/algorithm>
#include <stdio.h>

#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of remif1_test:\n");

  int numbers [6] = {0,0,1,1,2,2};

  remove_if ((int*)numbers,(int*)numbers+6,odd);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);
    
  printf ("\n");  

  return 0;
}
