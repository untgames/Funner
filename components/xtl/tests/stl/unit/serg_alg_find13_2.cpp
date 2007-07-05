//Тестирование алгоритма count_if (first, last, pred)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find13_2:\n");

  const int N = 10;
  int A[N] = {0,-1,-1,-1,1,-2,2,-3,3,4};
  int n;

  print ("A =", A, A+N);

  n = count_if (A, A+N, negative);
  printf ("Number of negative elements: %d\n", n);
  n = count_if (A, A+N, positive);
  printf ("Number of positive elements: %d\n", n);

  return 0;
}

