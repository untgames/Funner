//Тестирование алгоритма lexicographical_compare_3way (range1, range2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find16_3:\n");

  int A1[] = {3, 1, 4, 2, 8, 5, 7};
  int A2[] = {3, 1, 4, 1, 5, 9, 3};
  int A3[] = {1, 2, 3, 4};
  int A4[] = {1, 2, 3, 4, 5};

  const int N1 = sizeof(A1) / sizeof(int);
  const int N2 = sizeof(A2) / sizeof(int);
  const int N3 = sizeof(A3) / sizeof(int);
  const int N4 = sizeof(A4) / sizeof(int);

  int C11 = lexicographical_compare_3way (A1, A1 + N1, A1, A1 + N1);
  int C12 = lexicographical_compare_3way (A1, A1 + N1, A2, A2 + N2);
  int C34 = lexicographical_compare_3way (A3, A3 + N3, A4, A4 + N4);

  printf ("A1[] and A1[]: %d\n", C11);
  printf ("A1[] and A2[]: %d\n", C12);
  printf ("A3[] and A4[]: %d\n", C34);

  return 0;
}

