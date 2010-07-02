//Тестирование алгоритма includes (range1,range2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

template<typename T, size_t N>
size_t array_size (T (&)[N])
{
  return N;
}

int main () 
{
  printf ("Results of alg_set01_1:\n");

  int A1[] = { 1, 2, 3, 4, 5, 6, 7 };
  int A2[] = { 1, 4, 7 };
  int A3[] = { 2, 7, 9 };
  int A4[] = { 1, 1, 2, 3, 5, 8, 13, 21 };
  int A5[] = { 1, 2, 13, 13 };
  int A6[] = { 1, 1, 3, 21 };

  printf ("A2 contained in A1: %s\n", includes (A1, A1 + array_size (A1),
                                                A2, A2 + array_size (A2)) ? "true" : "false");
  printf ("A3 contained in A1: %s\n", includes (A1, A1 + array_size (A1),
                                                A3, A3 + array_size (A3)) ? "true" : "false");
  printf ("A5 contained in A4: %s\n", includes (A4, A4 + array_size (A4),
                                                A5, A5 + array_size (A5)) ? "true" : "false");
  printf ("A6 contained in A4: %s\n", includes (A4, A4 + array_size (A4),
                                                A6, A6 + array_size (A6)) ? "true" : "false");

  return 0;
}

