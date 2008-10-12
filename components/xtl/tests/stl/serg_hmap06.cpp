// Тестирование функции count
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_hmap6_test:\n");

  hash_map <int, int> hm1;
  hash_map <int, int>::size_type i;

  hm1.insert_pair (1, 1);
  hm1.insert_pair (2, 1);
  hm1.insert_pair (1, 2);
  hm1.insert_pair (2, 2);

  // Keys must be unique in hash_map, so duplicates are ignored
  i = hm1.count (1);
  printf ("Elemets with key of 1: %lu\n", i);

  i = hm1.count (2);
  printf ("Elemets with key of 2: %lu\n", i);

  i = hm1.count (3);
  printf ("Elemets with key of 3: %lu\n", i);

  return 0;
}
