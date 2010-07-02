// Тестирование функции count
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of map5_test:\n");

  map <int, int> m1;
  map <int, int>::size_type i;

  m1.insert_pair (1, 1);
  m1.insert_pair (2, 1);
  m1.insert_pair (1, 4);
  m1.insert_pair (2, 1);

  print ("m1 =", m1.begin (), m1.end ());

  i = m1.count (1); // Keys must be unique in map, so duplicates are ignored
  printf ("The number of elements in m1 with a sort key of 1 is: %lu\n", i);

  i = m1.count (2);
  printf ("The number of elements in m1 with a sort key of 2 is: %lu\n", i);

  i = m1.count (3);
  printf ("The number of elements in m1 with a sort key of 3 is: %lu\n", i);

  return 0;
}
