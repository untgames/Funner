// Тестирование erase и clear
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_list5_test:\n");

  list <int> c1;

  for (int i=10; i<60; i+=10)
    c1.push_back (i);

  print ("c1 =", c1.begin (), c1.end ());

  c1.erase (c1.begin ());
  printf ("After erasing the first element, the list becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());

  c1.erase (++c1.begin (), c1.end ());
  printf ("After erasing all elements but the first, the list becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());

  printf ("The size of the list is %u\n", c1.size ());
  c1.clear ();
  printf ("The size of list after clearing is %u\n", c1.size ());

  return 0;
}
