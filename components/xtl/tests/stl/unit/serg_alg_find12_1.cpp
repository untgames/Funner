//Тестирование алгоритма equal_range (first, last, value)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

void check (const int* first, const int* last, int value)
{
  pair <const int*, const int*> res = equal_range (first, last, value);
    
  if (res.first == res.second) printf ("no equal range for value: %d\n",value);
  else                         printf ("equal range for value %d: (%d,%d)\n",value,res.first-first,res.second-first);
}

int main () 
{
  printf ("Results of serg_alg_find12_1:\n");

  const int N = 10;
  int A[N] = {-3,-2,-1,-1,0,0,1,2,3,4};

  print ("A =", A, A+N);

  check (A, A+N, 3);
  check (A, A+N, -3);
  check (A, A+N, 0);
  check (A, A+N, 4);
  check (A, A+N, 5);

  return 0;
}
