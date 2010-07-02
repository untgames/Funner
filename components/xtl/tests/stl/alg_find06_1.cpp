//Тестирование алгоритма find_end (range1, range2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_find06_1:\n");

  const int N1 = 10;
  const int N2 = 4;
  int v1[N1] = {0,5,10,15,20,0,5,10,15,20};
  int v2[N2] = {5,10,15,20};
  int* result;

  print ("v1 =", v1, v1+N1);
  print ("v2 =", v2, v2+N2);

  result = find_end (v1, v1+N1, v2, v2+N2);

  if (result == v1+N1)
     printf ("There is no match of v2 in v1.\n");
  else
     printf ("There is match of v2 in v1 at position %d.\n", result - v1);

  return 0;
}

