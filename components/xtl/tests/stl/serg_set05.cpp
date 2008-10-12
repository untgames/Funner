// Тестирование count ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_set5_test:\n");

  set <int> s1;
  set <int>::size_type i;

  s1.insert (1);
  s1.insert (1);

  print ("s1 =", s1.begin (), s1.end ());

  // Keys must be unique in set, so duplicates are ignored
  i = s1.count (1);
  printf ("count (1) => %lu\n", i);

  i = s1.count (2);
  printf ("count (2) => %lu\n", i);

  return 0;
}
