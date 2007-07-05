#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of findif0_test:\n");

  int numbers [6] = {2,4,8,15,32,64};

  int* location = find_if (numbers,numbers+6,odd);
  
  if (location != numbers + 6)
    printf ("Value %d at offset %d is odd\n",*location,location-numbers);

  return 0;
}
