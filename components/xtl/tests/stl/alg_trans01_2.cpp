//Тестирование алгоритма transform (first1, last1, first2, result, op)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_trans01_2:\n");

  int A[] = { -4, -3, -2, -1, 0, 1, 2 };
  const int N = sizeof (A) / sizeof (N);

  printf ("Before transform:\n");
  print ("A =", A, A+N);

  transform (A, A+N, A, A, plus<int>());

  printf ("After transform:\n");
  print ("A =", A, A+N);

  return 0;
}

