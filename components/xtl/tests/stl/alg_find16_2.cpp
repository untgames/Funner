//Тестирование алгоритма lexicographical_compare (range1, range2, compare)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find16_2:\n");

  int A1[] = {3, 1, 4, 1, 5, 9, 3};
  int A2[] = {3, -1, 4, -1, 5, -9, 3};
  int A3[] = {1, 2, 3, 4};
  int A4[] = {1, -2, 3, -4, 5};

  const int N1 = sizeof (A1) / sizeof (int);
  const int N2 = sizeof (A2) / sizeof (int);
  const int N3 = sizeof (A3) / sizeof (int);
  const int N4 = sizeof (A4) / sizeof (int);

  bool C12 = lexicographical_compare (A1, A1 + N1, A2, A2 + N2, abs_less);
  bool C34 = lexicographical_compare (A3, A3 + N3, A4, A4 + N4, abs_less);

  printf ("A1[] < A2[]: %s\n", C12 ? "true" : "false");
  printf ("A3[] < A4[]: %s\n", C34 ? "true" : "false");

  return 0;
}

