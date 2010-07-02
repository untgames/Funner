//Тестирование алгоритма swap (a,b)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_swap01_1:\n");

  int x = 1,
      y = 2;

  printf ("Before swap: x=%d y=%d\n",x,y);
  swap (x, y);
  printf ("After swap:  x=%d y=%d\n",x,y);

  return 0;
}

