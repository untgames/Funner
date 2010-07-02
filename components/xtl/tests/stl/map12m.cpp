// Тестирование insert
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of map12m_test:\n");

  multimap <int, int> m1, m2;

  m1.insert_pair (1, 10);
  m1.insert_pair (2, 20);
  m1.insert_pair (3, 30);

  print ("m1 =", m1.begin (), m1.end ());

  m1.insert_pair (1, 10);
  m1.insert (--m1.end (), pair<int,int> (4, 40));

  print ("m1 =", m1.begin (), m1.end ());

  m2.insert (pair<int,int> (10, 100));
  m2.insert (++m1.begin (), --m1.end ());

  print ("m2 =", m2.begin (), m2.end ());

  return 0;
}
