// Тестирование remove
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of list12_test:\n");

  list<int> c1;

  c1.push_back (5);
  c1.push_back (100);
  c1.push_back (5);
  c1.push_back (200);
  c1.push_back (5);
  c1.push_back (300);

  print ("c1 =", c1.begin (), c1.end ());
  printf ("Size before remove: %lu\n", c1.size ());

  c1.remove (5);

  printf ("After removing elements with value 5, the list becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());
  printf ("Size after remove: %lu\n", c1.size ());

  return 0;
}
