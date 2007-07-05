#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of find0_test:\n");
  
  int numbers [10] = {0,1,4,9,16,25,36,49,64};

  printf ("Found 25 at offset %d\n",find (numbers,numbers+10,25)-numbers);

  return 0;
}
