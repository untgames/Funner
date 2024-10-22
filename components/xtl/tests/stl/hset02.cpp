// Тестирование функций общего назначения: size, max_size, empty, ...
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of hset2_test:\n");

  hash_set <int> hs1;

  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (30);
  hs1.insert (40);

  print ("hs1 =", hs1.begin (), hs1.end ());

  printf ("max_size correct: %d\n", hs1.max_size () == hash_set<int>::size_type (-1) / sizeof (hash_set<int>::allocator_type::value_type));
  printf ("size: %lu\n", hs1.size ());
  printf ("empty: %s\n", hs1.empty ()?"true":"false");

  hash_set <int>::key_equal ke = hs1.key_eq ();
  printf ("equal (10, 10) return '%s'\n", ke (10, 10)?"true":"false");
  printf ("equal (10, 20) return '%s'\n", ke (10, 20)?"true":"false");

  hash_set <int>::hasher hash = hs1.hash_func ();
  printf ("hash (10) return '%lu'\n", hash (10));

  return 0;
}

