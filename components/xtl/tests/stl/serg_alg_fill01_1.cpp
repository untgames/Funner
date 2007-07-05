//Тестирование алгоритма fill (first, last, val)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_fill01_1:\n");

  list <int> l1 (4);
  
  fill (l1.begin (), l1.end (), 4);

  print ("l1 =", l1.begin (), l1.end ());

  return 0;
}

