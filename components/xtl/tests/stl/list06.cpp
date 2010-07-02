// Тестирование push_back и pop_back
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list6_test:\n");

  list<int> c1;

  c1.push_back (1);
  c1.push_back (2);
  c1.push_back (3);

  print ("c1 =", c1.begin (), c1.end ());
  printf ("The first element is: %d\n", c1.front ());
  printf ("The last element is: %d\n", c1.back ());

  if (!c1.empty ()) {
    c1.pop_back ();
  }
  printf ("After pop_back the last element is: %d\n", c1.back ());

  return 0;
}
