//Тестирование алгоритма equal (first1, last1, first2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find14_1:\n");

  int A1[] = { 3, 1, 4, 1, 5, 9, 3 };
  int A2[] = { 3, 1, 4, 2, 8, 5, 7 };
  const int N = sizeof(A1) / sizeof(int);

  print ("A1 =", A1, A1+N);
  print ("A2 =", A2, A2+N);

  printf ("Result of comparison (A1,A2): %d\n", equal (A1, A1+N, A2));
  printf ("Result of comparison (A1,A1): %d\n", equal (A1, A1+N, A1));

  return 0;
}

