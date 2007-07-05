//Тестирование алгоритма prev_permutation (first,last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_perm02_1:\n");

  int A[] = {2, 3, 4, 5, 6, 1};
  const int N = sizeof(A) / sizeof(int);

  printf ("Initially              ");
  print ("A =", A, A+N);

  prev_permutation (A, A+N);
  printf ("After prev_permutation ");
  print ("A =", A, A+N);

  next_permutation (A, A+N);
  printf ("After next_permutation ");
  print ("A =", A, A+N);

  return 0;
}

