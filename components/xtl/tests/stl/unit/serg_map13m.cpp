// Тестирование swap
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_map13m_test:\n");

  multimap <int, int> m1, m2, m3;

  m1.insert_pair (1, 10);
  m1.insert_pair (2, 20);
  m1.insert_pair (3, 30);
  m2.insert_pair (10, 100);
  m2.insert_pair (20, 200);
  m3.insert_pair (20, 200);

  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());
  print ("m3 =", m3.begin (), m3.end ());

  m1.swap (m2);
  printf ("Result of 'm1.swap (m2)':\n");
  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());

  swap (m1, m3);
  printf ("Result of 'swap (m1, m3)':\n");
  print ("m1 =", m1.begin (), m1.end ());
  print ("m3 =", m3.begin (), m3.end ());

  m1.clear ();
  m2.clear ();
  m1.swap (m2);
  printf ("Result swap of empty maps':\n");
  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());

  return 0;
}
