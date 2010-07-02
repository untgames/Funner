// Тестирование информации о пакетах и resize ()
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of hmap13_test:\n");

  hash_map<Char,int> hm1 (2);

  hm1.insert_pair ('a',1);
  hm1.insert_pair ('b',2);
  printf ("size before resize = %lu\n", hm1.size ());
  hm1.resize (hm1.size () * 100); //размер чего изменяется???
  printf ("size after resize = %lu\n", hm1.size ());
  hm1.insert_pair ('c',3);
  hm1.insert_pair ('d',4);
  printf ("size after insertions = %lu\n", hm1.size ());

  printf ("bucket_count: %lu\n", hm1.bucket_count ());
  printf ("max_bucket_count: %lu\n", hm1.bucket_count ());
  printf ("elems_in_bucket: %lu\n", hm1.elems_in_bucket (0));

  print ("hm1 =", hm1.begin (), hm1.end ());

  return 0;
}
