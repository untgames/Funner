//Тестирование алгоритма search_n (range1, range2, pred)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

bool eq_nosign (int x, int y)
{
  return abs (x) == abs (y);
}

void lookup (int* first, int* last, size_t count, int val, bool (*pred) (int x, int y))
{
  printf ("Searching for a sequence of %d '%d'%s", count, val, (count != 1 ? "s: " : ":  "));
  int* result = search_n (first, last, count, val, pred);
  if (result == last)
    printf ("Not found\n");
  else
    printf ("Index = %d\n", result - first);
}

int main () 
{
  printf ("Results of serg_alg_find07_2:\n");

  const int N = 10;
  int A[N] = {1, 2, 1, 1, 3, -3, 1, 1, 1, 1};

  print ("A =", A, A+N);

  lookup (A, A+N, 1, 3, eq_nosign);
  lookup (A, A+N, 2, 3, eq_nosign);

  return 0;
}

