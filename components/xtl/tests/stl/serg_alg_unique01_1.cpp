//Тестирование алгоритма unique (first,last)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_unique01_1:\n");

  int A[] = { 5, -5, 5, -5, 5, -5, 5, -5, 4, 4, 4, 4, 7 };
  const int N = sizeof (A) / sizeof (int);
  
  printf ("Before unique:\n");
  print ("A =", A, A+N);

  int* new_end = unique (A, A+N);

  printf ("After unique:\n");
  print ("A =", A, new_end);

  return 0;
}

