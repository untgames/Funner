//Тестирование алгоритма stable_sort (first, last)
#include <stdio.h>
#include <ctype.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

inline bool less_nocase (char a, char b) { return toupper (a) < toupper (b); }

int main () 
{
  printf ("Results of serg_alg_sort05_2:\n");

  char A[] = "fdBeACFDbEac";
  const int N = sizeof(A) - 1;

  print ("Before sort: A =", A, A+N);
  stable_sort (A, A+N, less_nocase);
  print ("After sort:  A =", A, A+N);

  return 0;
}

