// Тестирование конструкторов
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hset1_test:\n");

  hash_set <int> hs1;

  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (30);
  hs1.insert (40);

  hash_set <int>::iterator iter = ++hs1.begin ();
  hash_set <int> hs2 (hs1.begin (), ++iter);

  hash_set <int>::size_type      size   = hash_set <int>::DEFAULT_TABLE_SIZE;
  hash_set <int>::hasher         hash   = hash_fun <int> ();
  hash_set <int>::key_equal      key_eq = equal_to <int> ();
  hash_set <int>::allocator_type alloc  = hs2.get_allocator ();

  hash_set <int> hs3 (++hs1.begin (), ++iter, size, hash, key_eq, alloc);

  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());
  print ("hs3 =", hs3.begin (), hs3.end ());

  return 0;
}
