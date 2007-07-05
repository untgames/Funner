// Тестирование конструкторов
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hmap1a_test:\n");

  hash_map <Char, int>* hm1 = new hash_map <Char, int>;

  hm1->insert_pair ('a', 10);
  hm1->insert_pair ('b', 20);
  hm1->insert_pair ('c', 30);

  print ("hm1 =", hm1->begin (), hm1->end ());

  delete hm1;

  printf ("obj count=%d\n", Char::obj_count);

  return 0;
}
