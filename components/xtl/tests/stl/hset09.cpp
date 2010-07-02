// Тестирование swap ()
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of hset09_test:\n");

  hash_set<int> hs1, hs2, hs3 (5);

  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (30);
  hs2.insert (100);
  hs2.insert (200);
  hs3.insert (1000);

  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());
  print ("hs2 =", hs3.begin (), hs3.end ());

  hs1.swap (hs2);
  printf ("Result of 'hs1.swap (hs2)':\n");
  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());

  swap (hs1, hs3);
  printf ("Result of 'swap (hs1, hs3)':\n");
  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs3 =", hs3.begin (), hs3.end ());

  return 0;
}
