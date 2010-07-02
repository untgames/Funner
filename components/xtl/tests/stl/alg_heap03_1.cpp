//Тестирование алгоритма pop_heap (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_heap03_1:\n");

  int A[] = {1, 2, 3, 4, 5, 6};
  const int N = sizeof (A) / sizeof (int);

  make_heap (A, A+N);
  print ("Before pop A =", A, A+N);

  pop_heap (A, A+N);
  print ("After pop A =", A, A+N-1);
  printf ("A[N-1] = %d\n", A[N-1]);

  return 0;
}

