//Тестирование алгоритма lower_bound (first, last, value, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (const int* first, const int* last, int value)
{
  const int* pos = lower_bound (first, last, value, abs_less);
  printf ("lowest position of value %d: %d\n", value, pos-first);
}

int main () 
{
  printf ("Results of serg_alg_find10_2:\n");

  const int N = 10;
  int A[N] = {-1,-1,0,0,1,1,2,2,3,3};

  print ("A =", A, A+N);

  check (A, A+N, 1);
  check (A, A+N, -1);

  return 0;
}

