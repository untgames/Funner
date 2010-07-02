//Тестирование алгоритма partial_sort (first, middle, last, comp)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_sort03_2:\n");

  int A[] = {7, 2, 6, 11, 9, 3, 12, 10, 8, 4, 1, 5};
  const int N = sizeof (A) / sizeof (int);

  print ("Before partial_sort: A =", A, A+N);
  partial_sort (A, A+5, A+N, greater<int> ());
  print ("After partial_sort: A =", A, A+N);

  return 0;
}

