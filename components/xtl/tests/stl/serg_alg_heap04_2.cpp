//Тестирование алгоритма sort_heap (first, last, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_heap04_2:\n");

  int A[] = {1, -4, 2, -8, 5, -7};
  const int N = sizeof (A) / sizeof (int);

  make_heap (A, A+N, abs_less);
  print ("Before sort A =", A, A+N);

  sort_heap (A, A+N, abs_less);
  print ("After sort A =", A, A+N);

  return 0;
}

