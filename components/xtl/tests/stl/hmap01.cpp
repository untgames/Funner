// Тестирование конструкторов hash_map
#include <stdio.h>
#include <stl/hash_map>
#include <stl/string>
#include "test.h"

using namespace stl;

int main () 
{
  typedef hash_map <string, int> MyHash;

  printf ("Results of hmap1_test:\n");

  MyHash hm0;
  hash_map <string, int, hash_fun<string>, equal_to<string> > hm1;

  hm1.insert_pair ("one", 0);
  hm1.insert_pair ("two", 10);
  hm1.insert_pair ("three", 20);
  hm1.insert_pair ("four", 30);
  hm1.insert_pair ("five", 40);

  hash_map <string, int, hash_fun <string> > hm2;
  hm2.insert_pair ("one", 10);
  hm2.insert_pair ("two", 20);

  MyHash::const_iterator first, last;
  first = hm1.begin ();
  last = ++hm1.begin ();
  MyHash hm3 (first, ++last);

  MyHash::size_type      size   = MyHash::DEFAULT_TABLE_SIZE;
  MyHash::hasher         hash   = hash_fun <string> (); //hm2.hash_func ();
  MyHash::key_equal      key_eq = equal_to <string> (); //hm2.key_eq ();
  MyHash::allocator_type alloc  = hm2.get_allocator ();

  hash_map <string, int > hm4 (++first, ++last, size, hash, key_eq, alloc);

  print ("hm0 =", hm0.begin (), hm0.end ());
  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());
  print ("hm3 =", hm3.begin (), hm3.end ());
  print ("hm4 =", hm4.begin (), hm4.end ());

  return 0;
}
