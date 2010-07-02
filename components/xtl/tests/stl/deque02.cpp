//тестирование функций общего назначения
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of deque02_test:\n");

  deque <int> c1, c2;

  c1.push_back (1);

  if (c1.empty ())
     printf ("The deque c1 is empty.\n");
  else
     printf ("The deque c1 is not empty.\n");

  if (c2.empty ())
     printf ("The queue c2 is empty.\n");
  else
     printf ("The queue c2 is not empty.\n");

  printf ("c1.size () = %lu\n", c1.size ());
  printf ("c2.size () = %lu\n", c2.size ());
  printf ("c1.max_size () = %lu\n", c1.max_size ());

  return 0;
}

