//тестирование operator[]
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_deque05_test:\n");

  deque <int> c1;

  c1.push_back (10);
  c1.push_back (20);
  printf ("The first integer of c1 is %d\n", c1[0]);
  int& i = c1[1];
  printf ("The second integer of c1 is %d\n", i);

  return 0;
}

