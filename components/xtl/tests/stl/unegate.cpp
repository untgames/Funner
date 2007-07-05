#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of unegate_test:\n");

  int array [3] = {1,2,3};

  int* p = find_if (array,array+3,not1 (ptr_fun (odd)));
  
  if (p != array+3)
    printf ("%d\n",*p);

  return 0;
}
