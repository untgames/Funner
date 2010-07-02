//Тестирование алгоритма equal (first1, last1, first2, pred)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find14_2:\n");

  int A1[] = { 6, 2, 8, 2, 10, 18, 6 };
  int A2[] = { 3, 1, 4, 1, 5, 9, 3 };
  const int N = sizeof(A1) / sizeof(int);

  print ("A1 =", A1, A1+N);
  print ("A2 =", A2, A2+N);

  printf ("Result of comparison (A1,A2): %d\n", equal (A2, A2+N, A1, twice));

  return 0;
}

