// Тестирование функции equal_range
#include <stdio.h>
#include <stl/hash_set>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  typedef hash_multiset<int> mySet;

  printf ("Results of serg_hset6m_test:\n");

  mySet hs1;
  
  hs1.insert (10);
  hs1.insert (20);
  hs1.insert (10);

  print ("hs1 =", hs1.begin (), hs1.end ());

  pair <mySet::const_iterator, mySet::const_iterator> p1, p2;
  p1 = hs1.equal_range (10);

  mySet::difference_type diff;
  diff = distance (p1.first, p1.second);

  printf ("Num of elements '10': %d\n", diff);
  printf ("Direct call to hs1.count (10): %d\n", hs1.count (10));

  return 0;
}
