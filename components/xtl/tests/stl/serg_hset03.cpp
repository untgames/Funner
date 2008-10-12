// Тестирование функций удаления: erase (), clear ()
#include <stdio.h>
#include <stl/hash_set>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_hset3_test:\n");

  hash_set<int> hs1, hs2, hs3;
  hash_set<int>::size_type n;
  int i;

  for (i=1; i<5; i++)
  {
    hs1.insert(i);
    hs2.insert(i * i);
    hs3.insert(i - 1);
  }

  print ("hs1 =", hs1.begin (), hs1.end ());
  print ("hs2 =", hs2.begin (), hs2.end ());
  print ("hs3 =", hs3.begin (), hs3.end ());

  hs1.erase (++hs1.begin ());

  printf ("After the 2nd element is deleted, the set hs1 is:\n");
  print ("hs1 =", hs1.begin (), hs1.end ());

  hs2.erase (++hs2.begin (), --hs2.end ());

  printf ("After the middle two elements are deleted, the set hs2 is:\n");
  print ("hs2 =", hs2.begin (), hs2.end ());

  n = hs3.erase (2);

  printf ("After the element with a key of 2 is deleted, the set hs3 is:\n");
  print ("hs3 =", hs3.begin (), hs3.end ());

  printf ("The size of the hs3 is %lu\n", hs3.size ());
  hs3.clear ();
  printf ("The size of hs3 after clearing is %lu\n", hs3.size ());

  return 0;
}
