#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of minelem1_test:\n");
  
  int numbers [6] = {-10,15,-100,36,-242,42};

  printf ("%d\n",*min_element (numbers,numbers+6));
  
  return 0;
}
