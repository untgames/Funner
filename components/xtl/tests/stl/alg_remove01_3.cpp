//Тестирование алгоритма remove (first, last, value)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_remove01_3:\n");

  int A[] = { 7, 1, 9, 2, 0, 7, 7, 3, 4, 6, 8, 5, 7, 7 };
  const int N = sizeof (A) / sizeof (int);
  
  printf ("Before remove:\n");
  print ("A =", A, A+N);

  int* new_end = remove (A, A+N, 7);

  printf ("After remove:\n");
  print ("A =", A, new_end);

  return 0;
}

