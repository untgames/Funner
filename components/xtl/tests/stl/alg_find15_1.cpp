//Тестирование алгоритма mismatch (first1, last1, first2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (int* first1, int* last1, int* first2)
{
  pair <int*, int*> result = mismatch (first1, last1, first2);
  if (result.first == last1)
     printf ("The two ranges do not differ.\n");
  else {
    printf ("Mismatch in position %d\n", result.first - first1);
    printf ("Values are: %d, %d\n", *result.first, *result.second);
  }
}

int main () 
{
  printf ("Results of alg_find15_1:\n");

  int A1[] = { 3, 1, 4, 1, 5, 9, 3 };
  int A2[] = { 3, 1, 4, 1, 5, 9, 7 };
  const int N = sizeof(A1) / sizeof(int);

  print ("A1 =", A1, A1+N);
  print ("A2 =", A2, A2+N);

  check (A1, A1+N, A1);
  check (A1, A1+N, A2);

  return 0;
}

