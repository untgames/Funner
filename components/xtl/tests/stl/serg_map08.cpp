// Тестирование функции equal_range
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  typedef map <int, int> myMap;

  printf ("Results of serg_map8_test:\n");

  myMap m1;
  myMap::const_iterator iter;

  m1.insert_pair (1,10);
  m1.insert_pair (2,20);
  m1.insert_pair (3,30);

  print ("m1 =", m1.begin (), m1.end ());

  pair <myMap::const_iterator, myMap::const_iterator> p1, p2;
  p1 = m1.equal_range (2);

  printf ("lower_bound (2): %d\n", p1.first->second);
  printf ("upper_bound (2): %d\n", p1.second->second);

  iter = m1.lower_bound (2);
  printf ("A direct call of lower_bound (2) gives %d\n", iter->second);
  iter = m1.upper_bound (2);
  printf ("A direct call of upper_bound (2) gives %d\n", iter->second);

  p2 = m1.equal_range (4);

  if ((p2.first  == m1.end ()) && 
      (p2.second == m1.end ()))
     printf ("Not Found!\n");
  else
     printf ("Found!\n");

  return 0;
}
