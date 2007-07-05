// Тестирование find ()
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_hset4m_test:\n");

  hash_multiset<int> hs1;
  hash_multiset<int>::const_iterator iter;
  
  hs1.insert (10);
  hs1.insert (10);
  hs1.insert (20);

  print ("hs1 =", hs1.begin (), hs1.end ());

  iter = hs1.find (10);
  printf ("First Found element: %d\n", *iter);

  iter = hs1.find (40);
  if (iter == hs1.end ())
    printf ("Element 40 Not Found!!!\n");
  else
    printf ("Element 40 Found???\n");

  iter = --hs1.end ();
  iter = hs1.find (*iter);
  printf ("First Found element: %d\n", *iter);

  return 0;
}
