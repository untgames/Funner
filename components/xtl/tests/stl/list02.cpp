// Тестирование assign
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list2_test:\n");

  list<int> c1, c2;
  
  for (int i=10; i<40; i+=10) {
    c1.push_back (i);
    c2.push_back (i+30);
  }
  print ("c1 =", c1.begin (), c1.end ());

  c1.assign (++c2.begin (), c2.end ());
  print ("c1 =", c1.begin (), c1.end ());

  c1.assign (7, 4);
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
