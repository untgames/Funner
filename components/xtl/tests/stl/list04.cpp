// Тестирование insert
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list4_test:\n");

  list <int> c1, c2;
  list <int>::iterator iter;

  for (int i=10; i<40; i+=10) {
    c1.push_back (i);
    c2.push_back (i+30);
  }

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());

  iter = ++c1.begin ();
  c1.insert (iter, 100);
  print ("c1 =", c1.begin (), c1.end ());

  iter = ++c1.begin ();
  c1.insert (++iter, 2, 200);
  print ("c1 =", c1.begin (), c1.end ());

  c1.insert (++c1.begin (), c2.begin (), --c2.end ());
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
