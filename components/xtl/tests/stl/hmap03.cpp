// Тестирование функций общего назначения: size, empty, ...
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main ()
{
  typedef hash_map <const char*, int> MyHash;

  printf ("Results of hmap3_test:\n");

  MyHash hm1;

  hm1.insert_pair ("serg",10);
  hm1.insert_pair ("leny",20);
  hm1.insert_pair ("jeny",30);
  hm1.insert_pair ("igor",40);

  print ("hm1 =", hm1.begin (), hm1.end ());
  printf ("size: %lu\n", hm1.size ());
  printf ("max_size: %lu\n", hm1.max_size ());
  printf ("empty: %s\n", hm1.empty ()?"true":"false");

  MyHash::key_equal ke = hm1.key_eq ();
  bool result = ke ("serg", "leny");
  printf ("ke ('serg', 'leny') return '%s'\n", result?"true":"false");

  MyHash::hasher hr = hm1.hash_func ();

  printf ("hash_func ('serg') return %lu\n", hr ("serg"));

  print ("hm1 =", hm1.begin (), hm1.end ());

  return 0;
}
