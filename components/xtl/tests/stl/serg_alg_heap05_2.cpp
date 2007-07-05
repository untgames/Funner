//Тестирование алгоритма is_heap (first, last, less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_heap05_2:\n");

  int A[] = {1, -4, 2, -8, 5, -7};
  const int N = sizeof (A) / sizeof (int);

  print ("A =", A, A+N);

  if (!is_heap (A, A+N, abs_less))
    printf ("A is not heap!\n");

  make_heap (A, A+N, abs_less);

  print ("A =", A, A+N);
  if (is_heap (A, A+N, abs_less))
    printf ("A is heap!\n");
  else
    printf ("A is not heap???\n");

  return 0;
}

