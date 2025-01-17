//Тестирование алгоритма is_sorted (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_sort06_1:\n");

  int A[] = {1, 4, 2, 8, 5, 7};
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);
  printf ("A sorted: %s.\n",is_sorted (A,A+N)?"YES":"NO");
  sort (A, A+N);
  print ("A =", A, A+N);
  printf ("A sorted: %s.\n",is_sorted (A,A+N)?"YES":"NO");

  return 0;
}

