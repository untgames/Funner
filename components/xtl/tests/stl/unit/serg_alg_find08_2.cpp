//Тестирование алгоритма min_element (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find08_2:\n");

  const int N = 10;
  int A[N] = {1, 2, 1, 1, 3, -3, 1, 1, 1, 1};
  char B[N] = {'d', 'e', 'f', 'a', 'x', 'l', 'q', 's', 'e', 'p'};

  print ("A =", A, A+N);
  print ("B =", B, B+N);

  int* m1 = min_element (A, A+N);
  char* m2 = min_element (B, B+N);

  printf ("min = %d\n", *m1);
  printf ("min = '%c'\n", *m2);

  return 0;
}

