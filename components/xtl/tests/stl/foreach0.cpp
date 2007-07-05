#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

inline void print_number (int x)
{
  print (x);
  print (' ');
}

int main ()
{
  printf ("Results of foreach0_test:\n");

  int numbers [10] = {1,1,2,3,5,8,13,21,34,55}; 

  for_each (numbers,numbers+10,ptr_fun(print_number));
  printf ("\n");
  
  return 0;
}
