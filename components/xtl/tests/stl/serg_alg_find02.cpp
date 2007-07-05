//Тестирование алгоритма find_if
#include <stdio.h>
#include <stl/list>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

inline bool greater5 (int value) { return value > 5; }
inline bool less5    (int value) { return value < 5; }

int main () 
{
  printf ("Results of serg_alg_find02:\n");

  list<int> l1;
  list<int>::iterator result;
  
  l1.push_back (5);
  l1.push_back (10);
  l1.push_back (15);
  l1.push_back (20);
  l1.push_back (10);

  print ("l1 =", l1.begin (), l1.end ());
  
  result = find_if (l1.begin (), l1.end (), greater5);

  if (result == l1.end ())
     printf ("Not found.\n");
  else
     printf ("Found: %d.\n", *result);

  result = find_if (l1.begin (), l1.end (), less5);

  if (result == l1.end ())
     printf ("Not found.\n");
  else
     printf ("Found: %d.\n", *result);

  return 0;
}

