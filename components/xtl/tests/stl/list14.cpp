// Тестирование unique
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list14_test:\n");

  list<int> c1;
  not_equal_to<int> mypred;
  
  c1.push_back (-10);
  c1.push_back (10);
  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (20);
  c1.push_back (-10);

  print ("c1 =", c1.begin (), c1.end ());
  
  list<int> c2 (c1);
  c2.unique ();
  printf ("After removing successive duplicate elements, ");
  print ("c2 =", c2.begin (), c2.end ());

  list <int> c3 (c2);
  c3.unique (mypred);
  printf ("After removing successive unequal elements, ");
  print ("c3 =", c3.begin (), c3.end ());

  return 0;
}
