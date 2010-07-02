//Тестирование алгоритма count (first, last, value, n)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (const int* first, const int* last, int value)
{
  int n = 0;
  count (first, last, value, n);
  printf ("Number of elements that match %d: %d\n", value, n);
}

int main () 
{
  printf ("Results of alg_find13_3:\n");

  const int N = 10;
  int A[N] = {0,0,-1,-1,1,-2,2,-3,3,4};

  print ("A =", A, A+N);

  check (A, A+N, 0);
  check (A, A+N, 1);
  check (A, A+N, 5);

  return 0;
}

