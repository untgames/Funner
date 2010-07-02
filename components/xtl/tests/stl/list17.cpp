// Тестирование sort
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list17_test:\n");

  list<char> c1;
  
  c1.push_back ('d');
  c1.push_back ('a');
  c1.push_back ('c');
  c1.push_back ('b');

  printf ("Before sorting: ");
  print ("c1 =", c1.begin (), c1.end ());

  c1.sort ();
  printf ("After sorting: ");
  print ("c1 =", c1.begin (), c1.end ());

  c1.sort (greater<char> ());
  printf ("After sorting with 'greater than' operation: ");
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
