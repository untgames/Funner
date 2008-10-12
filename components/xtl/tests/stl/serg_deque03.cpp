//тестирование функций доступа
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_deque03_test:\n");

  deque <int> c1;

  c1.insert (c1.begin (), 0);
  c1.push_back (-1);
  c1.push_back (-2);
  c1.push_back (-3);
  c1.push_front (1);
  c1.push_front (2);
  c1.push_front (3);

  while (c1.back ())
  {
    printf ("%d ", c1.back ());
    c1.pop_back ();
  }

  while (c1.front ())
  {
    printf ("%d ", c1.front ());
    c1.pop_front ();
  }

  printf ("\nc1.size () = %lu\n", c1.size ());

  return 0;
}

