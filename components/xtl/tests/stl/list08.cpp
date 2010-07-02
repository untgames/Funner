// Тестирование pop_front
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list8_test:\n");

  list<int> c1;

  c1.push_back (1);
  c1.push_back (2);
  c1.push_back (3);

  print ("c1 =", c1.begin (), c1.end ());
  printf ("The first element is: %d\n", c1.front ());
  printf ("The last element is: %d\n", c1.back ());

  c1.pop_front ();
  printf ("After pop_front the first element is: %d\n", c1.front ());

  return 0;
}
