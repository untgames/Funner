// Тестирование swap
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list10_test:\n");

  list<int> c1, c2, c3;

  c1.push_back (1);
  c1.push_back (2);
  c1.push_back (3);
  c2.push_back (10);
  c2.push_back (20);
  c3.push_back (100);

  print ("c1 =", c1.begin (), c1.end ());

  c1.swap (c2);
  printf ("After swapping with c2, list c1 is:\n");
  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());

  swap (c1, c3);
  printf ("After swapping with c3, list c1 is:\n");
  print ("c1 =", c1.begin (), c1.end ());
  print ("c3 =", c3.begin (), c3.end ());

  return 0;
}
