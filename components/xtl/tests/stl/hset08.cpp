// Тестирование insert ()
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of set08_test:\n");

  hash_set<int> hs1, hs2;

  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (30);
  hs1.insert (40);

  print ("hs1 =", hs1.begin (), hs1.end ());

  pair <hash_set<int>::iterator, bool> pr;
  pr = hs1.insert (10);

  if (pr.second == true)   
    printf ("The element 10 was inserted in hs1 successfully\n");
  else   
    printf ("The element 10 already exists: *(pr.first) = %d\n", *(pr.first));

  print ("hs1 =", hs1.begin (), hs1.end ());

  hs2.insert (100);
  hs2.insert (++hs1.begin (), --hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());

  return 0;
}
