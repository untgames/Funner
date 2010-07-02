// Тестирование resize
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of list9_test:\n");

  list<int> c1;

  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);

  print ("c1 =", c1.begin (), c1.end ());

  c1.resize (4, 40);
  printf ("The size of c1 is %lu\n", c1.size ());
  print ("c1 =", c1.begin (), c1.end ());

  c1.resize (5);
  printf ("The size of c1 is now %lu\n", c1.size ());
  print ("c1 =", c1.begin (), c1.end ());

  c1.resize (2);
  printf ("The reduced size of c1 is: %lu\n", c1.size ());
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
