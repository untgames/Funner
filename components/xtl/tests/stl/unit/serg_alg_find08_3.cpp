//Тестирование алгоритма max_element (first, last, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find08_3:\n");

  const int N = 10;
  int A[N] = {1, 2, 1, 1, 3, -3, -4, 1, 1, 1};

  print ("A =", A, A+N);

  int* m = max_element (A, A+N, abs_less);

  printf ("max = %d\n", *m);

  return 0;
}

