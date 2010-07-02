//тестирование функции assign и operator=
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of deque08_test:\n");

  deque <int> c1, c2;
  
  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);
  c2.push_back (40);
  c2.push_back (50);
  c2.push_back (60);

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());

  c1.assign (++c2.begin (), c2.end ());
  print ("c1 =", c1.begin (), c1.end ());

  c1.assign (7, 4);
  print ("c1 =", c1.begin (), c1.end ());

  c2 = c1;
  print ("c2 =", c2.begin (), c2.end ());

  return 0;
}

