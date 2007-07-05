// Тестирование функции lower_bound
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_map6_test:\n");

  map <int, int> m1;
  map <int, int>::const_iterator iter1, iter2;

  m1.insert_pair (1,10);
  m1.insert_pair (2,20);
  m1.insert_pair (3,30);

  print ("m1 =", m1.begin (), m1.end ());

  iter1 = m1.lower_bound (2);
  printf ("lower_bound (2): %d\n", iter1->second);

  iter1 = m1.lower_bound (4);
  if (iter1 == m1.end ())
     printf ("Not found!\n");
  else
     printf ("Found?!: %d\n", iter1->second);

  iter2 = --m1.end ();
  iter1 = m1.lower_bound (iter2->first);
  printf ("lower_bound (%d): %d\n", iter2->first, iter1->second);

  return 0;
}
