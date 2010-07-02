//Тестирование алгоритма median (a,b,c)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find17_3:\n");

  int x = median (2,3,1);
  float f = median (2.0f,3.0f,1.0f);
  char c = median ('b','c','a');

  printf ("median (2,3,1) = %d\n", x);
  printf ("median (2.0,3.0,1.0) = %.1f\n", f);
  printf ("median ('b','c','a') = '%c'\n", c);

  return 0;
}

