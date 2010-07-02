//Тестирование алгоритма binary_search (first, last, value, pred)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (const int* first, const int* last, int value)
{
  bool found = binary_search (first, last, value, abs_less);

  if (!found)
    printf ("Value %d not found.\n", value);
  else
    printf ("Value %d found.\n", value);
}

int main () 
{
  printf ("Results of alg_find09_2:\n");

  const int N = 10;
  int A[N] = {0,1,2,3,4,5,6,7,8,9};

  print ("A =", A, A+N);

  check (A, A+N, -1);
  check (A, A+N, 9);
  check (A, A+N, 10);


  return 0;
}

