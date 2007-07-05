//Тестирование алгоритма make_heap (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_heap01_1:\n");

  int A[] = {1, 4, 2, 8, 5, 7};
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  make_heap (A, A+N);

  print ("A =", A, A+N);

  return 0;
}

