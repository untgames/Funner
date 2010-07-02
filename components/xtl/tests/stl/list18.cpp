// Тестирование операторов сравнения
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list18_test:\n");

  list<char> c1, c2;

  c1.push_back ('a');
  c1.push_back ('b');
  c2.push_back ('c');
  c2.push_back ('d');

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
