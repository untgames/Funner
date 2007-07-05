// Тестирование reverse
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_list16_test:\n");

  list<int> c1;
  
  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);

  print ("c1 =", c1.begin (), c1.end ());

  c1.reverse ();

  printf ("Reversed ");
  print ("c1 =", c1.begin (), c1.end ());

  return 0;
}
