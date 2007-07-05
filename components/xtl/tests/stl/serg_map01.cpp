// Тестирование конструкторов
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_map1_test:\n");

  map <int, int> m0;              // Create an empty map m0 of key type integer

  map <int, int, less<int> > m1;  // Create an empty map m1 with the key comparison function of less than, then insert 4 elements
  m1.insert_pair (1, 10);
  m1.insert_pair (2, 20);
  m1.insert_pair (3, 30);
  m1.insert_pair (4, 40);

  map <int, int, greater<int> > m2;  // Create an empty map m2 with the key comparison function of geater than, then insert 2 elements
  m2.insert_pair (1, 10);
  m2.insert_pair (2, 20);

  map <int, int> m3 (m1);  // Create a copy, map m3, of map m1

  // Create a map m4 by copying the range m1[First, Last)
  map <int, int>::const_iterator first, last;
  first = m1.begin ();
  last = ++m1.begin ();
  map <int, int> m4 (first, ++last);

  // Create a map m5 by copying the range m4[First, Last) and with the allocator of map m2
  map <int, int>::allocator_type m2_Alloc = m2.get_allocator ();
  map <int, int> m5 (m4.begin (), ++m4.begin (), m2_Alloc);

  print ("m0 =", m0.begin (), m0.end ());
  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());
  print ("m3 =", m3.begin (), m3.end ());
  print ("m4 =", m4.begin (), m4.end ());
  print ("m5 =", m5.begin (), m5.end ());

  return 0;
}
