// Тестирование функций общего назначения: size, max_size, empty, ...
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main ()
{
  typedef map <int, int> myMap;

  printf ("Results of map2_test:\n");

  myMap m1;

  m1.insert_pair (1,10);
  m1.insert_pair (2,20);
  m1.insert_pair (3,30);
  m1.insert_pair (4,40);

  print ("m1 =", m1.begin (), m1.end ());
  printf ("max_size: %lu\n", m1.max_size ());
  printf ("size: %lu\n", m1.size ());
  printf ("empty: %s\n", m1.empty ()?"true":"false");

  myMap::key_compare kc = m1.key_comp ();
  bool result = kc (2, 3);
  printf ("kc (2, 3) return '%s'\n", result?"true":"false");

  myMap::value_type val1 (5, 50);
  myMap::value_type val2 (2, 20);
  myMap::value_compare vc = m1.value_comp ();

  result = vc (val1, val2);
  printf ("vc (val1, val2) return '%s'\n", result?"true":"false");

  m1.insert (val1);
  print ("m1 =", m1.begin (), m1.end ());

  return 0;
}
