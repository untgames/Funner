//Тестирование алгоритма upper_bound (first, last, value)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (const int* first, const int* last, int value)
{
  const int* pos = upper_bound (first, last, value);
  printf ("highest position of value %d: %d\n", value, pos-first);
}

int main () 
{
  printf ("Results of alg_find11_1:\n");

  const int N = 10;
  int A[N] = {0,1,1,2,2,3,4,4,5,5};

  print ("A =", A, A+N);

  check (A, A+N, 3);
  check (A, A+N, 5);
  check (A, A+N, -1);

  return 0;
}

