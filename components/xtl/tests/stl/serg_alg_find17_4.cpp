//Тестирование алгоритма min (a,b,less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find17_4:\n");

  int x = min (-2, 1, abs_less);

  printf ("min (-2, 1, abs_less) = %d\n", x);

  return 0;
}

