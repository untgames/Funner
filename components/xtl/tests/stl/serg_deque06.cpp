//тестирование функции erase и clear
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_deque06_test:\n");

  deque <int> c1;
  
  for (int i=10; i<60; i+=10)
    c1.push_back (i);

  print ("c1 =", c1.begin (), c1.end ());

  c1.erase (c1.begin ());

  printf ("After erasing the first element, the deque becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());

  c1.erase (++c1.begin (), c1.end ());
  printf ("After erasing all elements but the first, deque becomes:\n");
  print ("c1 =", c1.begin (), c1.end ());

  printf ("size before clear(): %u\n", c1.size ());
  c1.clear ();
  printf ("size after clear(): %u\n", c1.size ());

  return 0;
}

