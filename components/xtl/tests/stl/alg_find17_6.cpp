//Тестирование алгоритма median (a,b,c,less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find17_6:\n");

  printf ("median (-3, 2,-1,abs_less) = %d\n", median (-3, 2,-1,abs_less));
  printf ("median (-2,-3, 1,abs_less) = %d\n", median (-2,-3, 1,abs_less));
  printf ("median (-1, 2,-3,abs_less) = %d\n", median (-1, 2,-3,abs_less));

  return 0;
}

