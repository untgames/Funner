//Тестирование алгоритма search (range1, range2)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find05_1:\n");

  char S1[] = "Hello, world!";
  char S2[] = "world";
  char S3[] = "WORLD";
  const int N1 = sizeof (S1) - 1;
  const int N2 = sizeof (S2) - 1;
  const int N3 = sizeof (S3) - 1;

  char* p = search (S1, S1+N1, S2, S2+N2);
  printf("Found at position %d\n", p-S1);

  p = search (S1, S1+N1, S3, S3+N3);
  if (p == S1+N1)
    printf ("Not found\n");
  else
    printf("Found at position %d\n", p-S1);

  return 0;
}

