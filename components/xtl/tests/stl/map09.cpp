// Тестирование операторов сравнения
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of map9_test:\n");

  map <int, int> m1, m2;

  for (int i=1; i<5; i++)
  {
    m1.insert_pair (i, i);
    m2.insert_pair (i, i*i);
  }

  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());

  printf ("m1 == m2 => %s\n", m1 == m2 ? "true" : "false");
  printf ("m1 != m2 => %s\n", m1 != m2 ? "true" : "false");
  printf ("m1 <  m2 => %s\n", m1 <  m2 ? "true" : "false");
  printf ("m1 >  m2 => %s\n", m1 >  m2 ? "true" : "false");
  printf ("m1 <= m2 => %s\n", m1 <= m2 ? "true" : "false");
  printf ("m1 >= m2 => %s\n", m1 >= m2 ? "true" : "false");

  return 0;
}
