//Тестирование алгоритма max (a,b,less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find17_5:\n");

  int x = max (-2, 1, abs_less);

  printf ("max (-2, 1, abs_less) = %d\n", x);

  return 0;
}

