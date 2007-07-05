//Тестирование алгоритма partial_sort_copy (range1, range2)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_sort03_3:\n");

  int A[] = {7, 2, 6, 11, 9, 3, 12, 10, 8, 4, 1, 5};
  const int N = sizeof(A) / sizeof(int);

  vector<int> v1 (5);

  print ("A  =", A, A+N);
  partial_sort_copy (A, A+N, v1.begin (), v1.end ());
  print ("v1 =", v1.begin (), v1.end ());

  return 0;
}

