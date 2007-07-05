// Тестирование merge
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_list15_test:\n");

  list <int> c1, c2, c3;
  
  c1.push_back (3);
  c1.push_back (6);
  c2.push_back (2);
  c2.push_back (4);
  c3.push_back (5);
  c3.push_back (1);
               
  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());

  c2.merge (c1);  // Merge c1 into c2 in (default=less) ascending order
  c2.sort (greater<int> ());
  printf ("After merging c1 with c2 and sorting with >: ");
  print ("c2 =", c2.begin (), c2.end ());

  c2.merge (c3, greater<int> ());
  printf ("After merging c3 with c2 according to the '>' comparison relation: ");
  print ("c2 =", c2.begin (), c2.end ());

  return 0;
}
