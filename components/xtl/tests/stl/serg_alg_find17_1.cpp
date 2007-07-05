//Тестирование алгоритма min (a,b)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find17_1:\n");

  int x = min (1,0);
  float f = min (1.0f,0.0f);
  char c = min ('b','a');

  printf ("min (1,0) = %d\n", x);
  printf ("min (1.0,0.0) = %.1f\n", f);
  printf ("min ('b','a') = '%c'\n", c);

  return 0;
}

