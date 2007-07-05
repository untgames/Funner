// Тестирование insert
#include <stdio.h>
#include <stl/map>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_map10_test:\n");

  typedef map <int, int> myMap;
  myMap m1, m2;

  m1.insert_pair (1, 10);
  m1.insert_pair (2, 20);
  m1.insert_pair (3, 30);
  m1.insert_pair (4, 40);

  print ("m1 =", m1.begin (), m1.end ());

  pair <myMap::iterator, bool> pr;
  pr = m1.insert_pair (1, 10);

  if (pr.second == true)
     printf ("Insert (1,10) successfull\n");
  else 
     printf ("Insert failed: key 1 exist! Value: %d\n", pr.first->second);

  m1.insert (--m1.end (), make_pair (5, 50));

  print ("m1 =", m1.begin (), m1.end ());
  
  m2.insert_pair (10, 100);
  m2.insert (++m1.begin (), --m1.end ());

  print ("m2 =", m2.begin (), m2.end ());

  return 0;
}
