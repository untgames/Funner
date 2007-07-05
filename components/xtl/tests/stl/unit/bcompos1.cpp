#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of bcompos1_test:\n");

  int array [6] = {-2,-1,0,1,2,3};

  int* p = find_if (array,array+6,compose2 (logical_and<bool> (),ptr_fun (odd),not1 (ptr_fun (negative))));
  
  if (p != array+6)
    printf ("%d is odd and positive\n",*p);

  return 0;
}
