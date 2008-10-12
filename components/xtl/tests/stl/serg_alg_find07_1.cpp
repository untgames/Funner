//Тестирование алгоритма search_n (range1, range2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void lookup (int* first, int* last, size_t count, int val)
{
  printf ("Searching for a sequence of %lu '%d'%s", count, val, (count != 1 ? "s: " : ":  "));
  int* result = search_n (first, last, count, val);
  if (result == last)
    printf ("Not found\n");
  else
    printf ("Index = %d\n", result - first);
}

int main ()
{
  printf ("Results of serg_alg_find07_1:\n");

  const int N = 10;
  int A[N] = {1, 2, 1, 1, 3, -3, 1, 1, 1, 1};

  print ("A =", A, A+N);

  lookup (A, A+N, 1, 4);
  lookup (A, A+N, 0, 4);
  lookup (A, A+N, 1, 1);
  lookup (A, A+N, 2, 1);
  lookup (A, A+N, 3, 1);
  lookup (A, A+N, 4, 1);
  lookup (A, A+N, 1, 3);
  lookup (A, A+N, 2, 3);

  return 0;
}

