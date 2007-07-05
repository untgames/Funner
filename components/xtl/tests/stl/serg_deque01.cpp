//тестирование конструкторов
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_deque01_test:\n");

  deque <int> c0;         // Create an empty deque c0
  deque <int> c1 (3);     // Create a deque c1 with 3 elements of default value 0
  deque <int> c2 (5, 2);  // Create a deque c2 with 5 elements of value 2
  deque <int> c3 (c2);    // Create a copy, deque c3, of deque c2

  // Create a deque c4 by copying the range c3[_First, _Last)
  deque <int>::iterator iter = ++c3.begin ();
  deque <int> c4 (c3.begin (), ++iter);

  // Create a deque c5 by copying the range from c2 and with the allocator of deque c4
  iter = ++c2.begin ();
  deque <int> c5 (c2.begin (), ++iter, c4.get_allocator () );

  print ("c0 =", c0.begin (), c0.end ());
  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());
  print ("c4 =", c4.begin (), c4.end ());
  print ("c5 =", c5.begin (), c5.end ());

  return 0;
}

