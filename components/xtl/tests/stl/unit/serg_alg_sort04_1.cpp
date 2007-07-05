//Тестирование алгоритма sort (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_sort04_1:\n");
  int A[] = {1, 4, 2, 8, 5, 7};
  const int N = sizeof (A) / sizeof (int);

  print ("Before sort: A =", A, A+N);
  sort (A, A+N);
  print ("After sort:  A =", A, A+N);

  return 0;
}

