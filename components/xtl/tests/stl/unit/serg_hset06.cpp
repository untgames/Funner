// Тестирование функции equal_range
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  typedef hash_set<int> mySet;

  printf ("Results of serg_hset6_test:\n");

  mySet hs1;
  
  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (30);

  print ("hs1 =", hs1.begin (), hs1.end ());

  pair <mySet::const_iterator, mySet::const_iterator> p1, p2;
  p1 = hs1.equal_range (20);

  printf ("lower_bound (20): %d\n", *(p1.first));
  printf ("upper_bound (20): %d\n", *(p1.second));

  p2 = hs1.equal_range (40);

  if ((p2.first  == hs1.end ()) && 
      (p2.second == hs1.end ()))
     printf ("Not Found!\n");
  else
     printf ("Found!\n");

  return 0;
}
