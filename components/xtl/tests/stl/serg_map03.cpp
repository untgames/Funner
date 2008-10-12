// Тестирование функций удаления: erase (), clear ()
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_map3_test:\n");

  map <int, int> m1, m2, m3;
  map <int, int>::size_type n;

  for (int i=1; i<5; i++)
  {
      m1.insert_pair (i, i);
      m2.insert_pair (i, i*i);
      m3.insert_pair(i, i-1);
  }

  print ("m1 =", m1.begin (), m1.end ());
  print ("m2 =", m2.begin (), m2.end ());
  print ("m3 =", m3.begin (), m3.end ());

  m1.erase (++m1.begin ());   // The 1st member function removes an element at a given position

  printf ("After the 2nd element is deleted, the map m1 is:\n");
  print ("m1 =", m1.begin (), m1.end ());

  m2.erase (++m2.begin (), --m2.end ());  // The 2nd member function removes elements in the range [_First, _Last)

  printf ("After the middle two elements are deleted, the map m2 is:\n");
  print ("m2 =", m2.begin (), m2.end ());

  n = m3.erase (2);  // The 3rd member function removes elements with a given _Key
  printf ("After the element with a key of 2 is deleted, the map m3 is:\n");
  print ("m3 =", m3.begin (), m3.end ());

  // The 3rd member function returns the number of elements removed
  printf ("The number of elements removed from m3 is: %lu.\n", n);

  // The dereferenced iterator can also be used to specify a key
  m3.erase (++m3.begin ());
  printf ("After the 2nd element is deleted, the map m1 is: \n");
  print ("m3 =", m3.begin (), m3.end ());

  printf ("The size of the m3 is %lu\n", m3.size ());
  m3.clear ();
  printf ("The size of the m3 after clear() is %lu\n", m3.size ());

  return 0;
}
