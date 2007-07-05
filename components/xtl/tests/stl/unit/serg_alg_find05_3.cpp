//Тестирование алгоритма search
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_find05_3:\n");

  const int N1 = 10;
  const int N2 = 2;
  int v1[N1] = {0,5,10,15,20,0,5,10,15,20};
  int v2[N2] = {15,20};
  int* result;

  print ("v1 =", v1, v1+N1);
  print ("v2 =", v2, v2+N2);

  result = search (v1, v1+N1, v2, v2+N2);

  if (result == v1+N1)
     printf ("No match of v2 in v1.\n");
  else
     printf ("Match of v2 in v1 at position %d.\n", result - v1);

  result = search (v1, v1+N1, v2, v2+N2, twice);

  if (result == v1+N1)
     printf ("No match of v2 in v1.\n");
  else
     printf ("Match of v2 in v1 at position %d.\n", result - v1);


  return 0;
}

