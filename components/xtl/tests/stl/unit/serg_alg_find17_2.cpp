//Тестирование алгоритма max (a,b)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find17_2:\n");

  int x = max (1,0);
  float f = max (1.0f,0.0f);
  char c = max ('b','a');

  printf ("max (1,0) = %d\n", x);
  printf ("max (1.0,0.0) = %.1f\n", f);
  printf ("max ('b','a') = '%c'\n", c);

  return 0;
}

