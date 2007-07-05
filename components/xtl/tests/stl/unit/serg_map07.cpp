// Тестирование функции upper_bound
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_map7_test:\n");

  map <int, int> m1;
  map <int, int>::const_iterator iter;

  m1.insert_pair (1,10);
  m1.insert_pair (2,20);
  m1.insert_pair (3,30);

  print ("m1 =", m1.begin (), m1.end ());

  iter = m1.upper_bound (2);
  printf ("upper_bound (2): %d\n", iter->second);

  // If no match is found for this key, end () is returned
  iter = m1.upper_bound (3);

  if (iter == m1.end ())
     printf ("Not found!\n");
  else
     printf ("Found?!: %d\n", iter->second);

  iter = m1.upper_bound (m1.begin()->first);
  printf ("upper_bound (%d): %d\n", m1.begin()->first, iter->second);

  return 0;
}
