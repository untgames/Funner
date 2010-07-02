//тестирование resize
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of deque10_test:\n");

  deque <int> c1;

  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);

  print ("c1 =", c1.begin (), c1.end ());

  c1.resize (4, 40);
  printf ("size of c1 is: %lu\n", c1.size ());
  printf ("last element is %d\n", c1.back ());

  c1.resize (5);
  printf ("size of c1 is now: %lu\n", c1.size ());
  printf ("last element is now %d\n", c1.back ());

  c1.resize (2);
  printf ("reduced size of c1 is: %lu\n", c1.size ());
  printf ("last element is now %d\n", c1.back ());

  return 0;
}
