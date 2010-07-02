//Тестирование алгоритма copy_backward (first, last, result)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_copy01_3:\n");

  int A[] = { 10, 20, 30, 40, 50 };
  const int N = sizeof (A) / sizeof (int);
  vector <int> v1 (N);
  
  print ("A  =", A, A+N);

  copy_backward (A, A+N, v1.begin () + N);

  print ("v1 =", v1.begin (), v1.end ());

  return 0;
}

