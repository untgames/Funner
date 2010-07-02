//Тестирование алгоритма adjacent_find 
#include <stdio.h>
#include <stl/list>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find04:\n");

  list <int> l1;
  list <int>::iterator result;
  
  l1.push_back (50);
  l1.push_back (40);
  l1.push_back (10);
  l1.push_back (20);
  l1.push_back (20);

  print ("l1 =", l1.begin (), l1.end ());

  result = adjacent_find (l1.begin (), l1.end ());

  if (result == l1.end ())
     printf ("Not found.\n");
  else
     printf ("Found. Value: %d.\n", *result);

  result = adjacent_find (l1.begin (), l1.end (), twice);

  if (result == l1.end ())
     printf ("Not found.\n");
  else {
     printf ("Found. Values: [%d,", *result);
     printf ("%d].\n", *(++result));
  }

  return 0;
}

