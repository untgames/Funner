#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of gener1_test:\n");
  
  int numbers [10];
  
  generate (numbers,numbers+10,myrand);

  for (int i=0;i<10;i++) printf ("%d ",numbers [i]);
  printf ("\n");
  
  return 0;
}
