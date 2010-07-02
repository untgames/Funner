//Тестирование алгоритма inplace_merge (first, middle, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_sort08_1:\n");

  int A[] = { 1, 3, 5, 7, 9, 2, 4, 6, 8 };
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  inplace_merge (A, A+5, A+N);

  print ("merged A =", A, A+N);

  return 0;
}

