// Тестирование конструкторов
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list1_test:\n");

  list<int> c0;         // Create an empty list c0
  list<int> c1 (3);     // Create a list c1 with 3 elements of default value 0
  list<int> c2 (5, 2);  // Create a list c2 with 5 elements of value 2
  list<int> c3 (c1);    // Create a copy, list c3, of list c2

  // Create a list c4 by copying the range c3
  list<int>::iterator iter = ++c3.begin ();
  list <int> c4 (c3.begin (), ++iter);

  // Create a list c5 by copying the range c2 and with the allocator of list c4
  iter = ++c2.begin ();
  list <int> c5 (c2.begin (), ++iter, c4.get_allocator ());

  print ("c0 =", c0.begin (), c0.end ());
  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());
  print ("c4 =", c4.begin (), c4.end ());
  print ("c5 =", c5.begin (), c5.end ());

  return 0;
}

