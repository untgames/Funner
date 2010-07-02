// Тестирование функции find
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of map4_test:\n");

  map <int, int> m1;
  map <int, int>::const_iterator iter;

  m1.insert_pair (1, 10);
  m1.insert_pair (2, 20);
  m1.insert_pair (3, 30);

  print ("m1 =", m1.begin (), m1.end ());

  iter = m1.find (2);
  printf ("The element of map m1 with a key of 2 is: %d.\n", iter->second);

  iter = m1.find (4);  // If no match is found for the key, end() is returned

  if (iter == m1.end ())
     printf ("The map m1 doesn't have an element with a key of 4.\n");
  else
     printf ("The element of map m1 with a key of 4 is: %d.\n", iter->second);

  iter = m1.find ((--m1.end ())->first);
  printf ("The element of m1 with a key matching that of the last element is: %d.\n", iter->second);

  return 0;
}
