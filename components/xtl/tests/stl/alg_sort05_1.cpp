//Тестирование алгоритма stable_sort (first, last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_sort05_1:\n");

  char A[] = "fdBeACFDbEac";
  const int N = sizeof(A) - 1;

  print ("Before sort: A =", A, A+N);
  stable_sort (A, A+N);
  print ("After sort:  A =", A, A+N);

  return 0;
}

