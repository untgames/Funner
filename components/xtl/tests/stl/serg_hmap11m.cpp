// Тестирование функции insert
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hmap11m_test:\n");

  hash_multimap <int,int> hm1, hm2;

  hm1.insert_pair (1, 10);
  hm1.insert_pair (2, 20);
  hm1.insert_pair (3, 30);

  print ("hm1 =", hm1.begin (), hm1.end ());

  hm1.insert_pair (1, 10);

  print ("hm1 =", hm1.begin (), hm1.end ());

  hm2.insert (make_pair (10, 100));
  hm2.insert (++hm1.begin (), --hm1.end ());

  print ("hm2 =", hm2.begin (), hm2.end ());

  return 0;
}
