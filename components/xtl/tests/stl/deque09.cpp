//тестирование swap
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of deque09_test:\n");

  deque <int> c1, c2, c3;

  c1.push_back (1);
  c1.push_back (2);
  c1.push_back (3);
  c2.push_back (10);
  c2.push_back (20);
  c3.push_back (100);

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());

  c1.swap (c2);

  printf ("After swapping with c2, deque ");
  print ("c1 =", c1.begin (), c1.end ());
 
  swap (c1, c3);

  printf ("After swapping with c3, deque ");
  print ("c1 =", c1.begin (), c1.end ());

  swap<> (c1, c2);
  printf ("After swapping with c2, deque ");
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}

