// Тестирование функций общего назначения: size, max_size, empty, ...
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_set2_test:\n");

  set <int, less <int> > s1;
  s1.insert (10);
  s1.insert (20);
  s1.insert (30);
  s1.insert (40);

  set <int, greater <int> > s2 (s1.begin (), s1.end ());

  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());
  printf ("max_size: %u\n", s1.max_size ());
  printf ("size: %u\n", s1.size ());
  printf ("empty: %s\n", s1.empty ()?"true":"false");

  set <int, less <int> >::key_compare lkc = s1.key_comp ();
  bool result = lkc (2, 3);
  printf ("lkc (2, 3) return '%s'\n", result?"true":"false");

  set <int, greater <int> >::key_compare gkc = s2.key_comp ();
  result = gkc (2, 3);
  printf ("gkc (2, 3) return '%s'\n", result?"true":"false");

  set <int, less <int> >::value_compare lvc = s1.value_comp ();
  result = lvc (40, 50);
  printf ("lvc (10, 20) return '%s'\n", result?"true":"false");

  set <int, greater <int> >::value_compare gvc = s2.value_comp ();
  result = gvc (40, 50);
  printf ("gvc (10, 20) return '%s'\n", result?"true":"false");

  return 0;
}
