// Тестирование функции find
#include <stdio.h>
#include <stl/hash_map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of hmap5m_test:\n");

  hash_multimap <int, int> hm1;
  hash_multimap <int, int>::const_iterator iter;

  hm1.insert_pair (1, 10);
  hm1.insert_pair (2, 20);
  hm1.insert_pair (3, 30);
  hm1.insert_pair (4, 30);

  print ("hm1 =", hm1.begin (), hm1.end ());

  iter = hm1.find (2);
  printf ("Element with a key of 2 is: %d.\n", iter->second);

  iter = hm1.find (5);

  if (iter == hm1.end ())
     printf ("Element with a key of 4 not found.\n");
  else
     printf ("Element with a key of 4 found: %d.\n", iter->second);

  iter = hm1.find ((--hm1.end ())->first);
  printf ("The element of hm1 with a key matching that of the last element is: %d.\n", iter->second);

  return 0;
}
