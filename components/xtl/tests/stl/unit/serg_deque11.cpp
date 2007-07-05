//тестирование resize
#include <stdio.h>
#include <stl/deque>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_deque11_test:\n");

  deque <int> c1, c2;

  c1.push_back (10);
  c1.push_back (20);
  c1.push_back (30);
  c2.push_back (30);
  c2.push_back (20);
  c2.push_back (10);

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());

  printf ("c1 == c2 => %s\n", c1 == c2 ? "true" : "false");
  printf ("c1 != c2 => %s\n", c1 != c2 ? "true" : "false");
  printf ("c1 <  c2 => %s\n", c1 <  c2 ? "true" : "false");
  printf ("c1 >  c2 => %s\n", c1 >  c2 ? "true" : "false");
  printf ("c1 <= c2 => %s\n", c1 <= c2 ? "true" : "false");
  printf ("c1 >= c2 => %s\n", c1 >= c2 ? "true" : "false");

  return 0;
}
