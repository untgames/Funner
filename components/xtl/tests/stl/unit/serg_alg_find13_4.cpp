//Тестирование алгоритма count_if (first, last, pred, n)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find13_4:\n");

  const int N = 10;
  int A[N] = {0,-1,-1,-1,1,-2,2,-3,3,4};
  int n;

  print ("A =", A, A+N);

  count_if (A, A+N, negative, n=0);
  printf ("Number of negative elements: %d\n", n);
  count_if (A, A+N, positive, n=0);
  printf ("Number of positive elements: %d\n", n);

  return 0;
}

