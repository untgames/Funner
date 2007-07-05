#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int my_random_number (int max)
{
  return myrand () % max;
}

int main ()
{
  printf ("Results of rndshuf0_test:\n");
  
  int numbers [6] = {1,2,3,4,5,6};

  random_shuffle (numbers,numbers+6,my_random_number);
  
  for (int i=0;i<6;i++) printf ("%d ",numbers [i]);
  printf ("\n");

  return 0;
}
