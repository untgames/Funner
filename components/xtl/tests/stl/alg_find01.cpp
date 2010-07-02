//Тестирование алгоритма find
#include <stdio.h>
#include <stl/list>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find01:\n");

  list <int> l1;
  list <int>::iterator result;

  l1.push_back (1);
  l1.push_back (2);
  l1.push_back (3);
  
  print ("l1 =", l1.begin (), l1.end ());
  
  result = find (l1.begin (), l1.end (), 3);

  if (result == l1.end ())
     printf ("Not found.\n");
  else
     printf ("Found: %d.\n", *result);

  result = find (l1.begin (), l1.end (), 4);

  if (result == l1.end ())
     printf ("Not found.\n");
  else
     printf ("Found: %d.\n", *result);


  return 0;
}

