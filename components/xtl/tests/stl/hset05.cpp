// Тестирование count ()
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main ()
{
  typedef hash_set<int> mySet;

  printf ("Results of hset5_test:\n");

  mySet hs1;
  mySet::size_type i;

  hs1.insert (1);
  hs1.insert (1);  // Keys must be unique in set, so duplicates are ignored

  print ("hs1 =", hs1.begin (), hs1.end ());

  i = hs1.count (1);
  printf ("count (1) => %lu\n", i);

  i = hs1.count (2);
  printf ("count (2) => %lu\n", i);

  return 0;
}
