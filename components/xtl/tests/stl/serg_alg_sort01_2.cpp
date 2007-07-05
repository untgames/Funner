//Тестирование алгоритма stable_partition (first, last, pred)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

inline bool greater5 (int value) { return value > 5; }

int main () 
{
  printf ("Results of serg_alg_sort01_2:\n");

  int A[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  int* pos = stable_partition (A, A+N, greater5);

  printf ("result position: %d\n", pos-A);
  print ("A =", A, A+N);

  return 0;
}

