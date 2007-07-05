// Тестирование операторов сравнения
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hset7_test:\n");

  hash_set<int> hs1, hs2;

  for (int i=1; i<5; i++)
  {
    hs1.insert (i);
    hs2.insert (i*i);
  }

  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());

  printf ("hs1 == hs2 => %s\n", hs1 == hs2 ? "true" : "false");
  printf ("hs1 != hs2 => %s\n", hs1 != hs2 ? "true" : "false");
  printf ("hs1 <  hs2 => %s\n", hs1 <  hs2 ? "true" : "false");
  printf ("hs1 >  hs2 => %s\n", hs1 >  hs2 ? "true" : "false");
  printf ("hs1 <= hs2 => %s\n", hs1 <= hs2 ? "true" : "false");
  printf ("hs1 >= hs2 => %s\n", hs1 >= hs2 ? "true" : "false");

  return 0;
}
