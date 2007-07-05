//Тестирование алгоритма nth_element (first, nth, last, comp)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_sort02_2:\n");

  int A[] = {7, 2, 6, 11, 9, 3, 12, 10, 8, 4, 1, 5};
  const int N = sizeof (A) / sizeof (int);

  print ("Before nth: A =", A, A+N);
  nth_element (A, A+6, A+N, greater<int> ());
  print ("After nth: A =", A, A+N);

  return 0;
}

