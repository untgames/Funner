//Тестирование алгоритма merge (range1, range2, result, less)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_sort07_2:\n");

  int A1[] = { 1, 3, 5, 7 };
  int A2[] = { -2, -4, -6, -8, -9, -10};
  const int N1 = sizeof(A1) / sizeof(int);
  const int N2 = sizeof(A2) / sizeof(int);

  print ("A1 =", A1, A1+N1);
  print ("A2 =", A2, A2+N2);

  vector <int> v1 (N1+N2);
  merge (A1, A1+N1, A2, A2+N2, v1.begin (), abs_less);

  print ("merged v1 =", v1.begin (), v1.end ());

  return 0;
}

