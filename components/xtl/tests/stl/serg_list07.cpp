// Тестирование push_front
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_list7_test:\n");

  list<int> c1;

//  printf ("DANGER: %d\n", c1.front ());
  
  c1.push_front (1);
  if (c1.size () != 0)
    printf ("First element: %d\n", c1.front ());

  c1.push_front (2);
  if (c1.size () != 0)
     printf ("New first element: %d\n", c1.front ());

  return 0;
}
