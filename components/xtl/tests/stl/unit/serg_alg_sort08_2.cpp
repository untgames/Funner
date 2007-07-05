//Тестирование алгоритма inplace_merge (first, middle, last, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_sort08_2:\n");

  int A[] = { 1, 3, 5, 7, 9, -2, -4, -6, -8 };
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  inplace_merge (A, A+5, A+N, abs_less);

  print ("merged A =", A, A+N);

  return 0;
}

