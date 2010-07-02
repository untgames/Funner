//Тестирование алгоритма search (range1, range2, pred)
#include <stdio.h>
#include <ctype.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

inline bool no_case (char a, char b) { return toupper (a) == toupper (b); }

int main () 
{
  printf ("Results of alg_find05_2:\n");

  char S1[] = "Hello, world!";
  char S2[] = "WORLD";
  const int N1 = sizeof (S1) - 1;
  const int N2 = sizeof (S2) - 1;

  const char* p = search (S1, S1+N1, S2, S2+N2, no_case);
  printf("Found at position %d\n", p-S1);

  return 0;
}

