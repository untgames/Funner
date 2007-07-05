// Тестирование функции swap
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hmap12m_test:\n");

  hash_multimap<Char,int> hm1, hm2, hm3;

  hm1.insert_pair ('a', 10);
  hm1.insert_pair ('b', 20);
  hm1.insert_pair ('c', 30);
  hm2.insert_pair ('d', 100);
  hm2.insert_pair ('e', 200);
  hm3.insert_pair ('f', 300);

  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());
  print ("hm3 =", hm3.begin (), hm3.end ());

  hm1.swap (hm2);
  printf ("Result of 'hm1.swap (hm2)':\n");
  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());

  swap (hm1, hm3);
  printf ("Result of 'swap (hm1, hm3)':\n");
  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm3 =", hm3.begin (), hm3.end ());

  hm1.clear ();
  hm2.clear ();
  hm1.swap (hm2);
  printf ("Result swap of empty maps':\n");
  print ("hm1 =", hm1.begin (), hm1.end ());
  print ("hm2 =", hm2.begin (), hm2.end ());

  printf ("obj count=%d\n",Char::obj_count);

  return 0;
}
