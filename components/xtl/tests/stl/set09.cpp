// Тестирование операторов сравнения
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of set9_test:\n");

  set <int> s1, s2;

  for (int i=1; i<5; i++)
  {
    s1.insert (i);
    s2.insert (i*i);
  }

  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());

  printf ("s1 == s2 => %s\n", s1 == s2 ? "true" : "false");
  printf ("s1 != s2 => %s\n", s1 != s2 ? "true" : "false");
  printf ("s1 <  s2 => %s\n", s1 <  s2 ? "true" : "false");
  printf ("s1 >  s2 => %s\n", s1 >  s2 ? "true" : "false");
  printf ("s1 <= s2 => %s\n", s1 <= s2 ? "true" : "false");
  printf ("s1 >= s2 => %s\n", s1 >= s2 ? "true" : "false");

  return 0;
}
