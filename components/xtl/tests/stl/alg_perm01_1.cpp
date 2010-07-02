//Тестирование алгоритма next_permutation (first,last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

template <class BidirectionalIterator> 
void snail_sort (BidirectionalIterator first, BidirectionalIterator last)
{
  while (next_permutation (first, last)) {}  
}

int main () 
{
  printf ("Results of alg_perm01_1:\n");

  int A[] = {8, 3, 6, 1, 2, 5, 7, 4};
  const int N = sizeof(A) / sizeof(int);

  snail_sort (A, A+N);  
  print ("A =", A, A+N);

  return 0;
}

