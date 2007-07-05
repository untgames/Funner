#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of maxelem1_test:\n");
  
  int numbers [6] = {4,10,56,11,-42,19};

  printf ("%d\n",*max_element (numbers,numbers+6));
  
  return 0;
}
