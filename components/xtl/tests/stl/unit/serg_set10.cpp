// Тестирование insert ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_set10_test:\n");

  set <int> s1, s2;
  s1.insert (10);
  s1.insert (20);
  s1.insert (30);
  s1.insert (40);

  print ("s1 =", s1.begin (), s1.end ());

  pair <set <int>::iterator, bool> pr;
  pr = s1.insert (10);

  if (pr.second == true)   
    printf ("The element 10 was inserted in s1 successfully\n");
  else   
    printf ("The element 10 already exists: *pr.first = %d\n", *pr.first);

  s1.insert (--s1.end (), 50);

  print ("s1 =", s1.begin (), s1.end ());

  s2.insert (100);
  s2.insert (++s1.begin (), --s1.end ());

  print ("s2 =", s2.begin (), s2.end ());

  return 0;
}
