//Тестирование алгоритма prev_permutation (first,last,less)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

inline bool just_less (int a, int b) { return a < b; }

void all_permutations (const char* title, int* first, int* last, bool (*less) (int,int))
{
  printf ("%s\n",title);
  do {
    print ("", first, last);
  } while (prev_permutation (first, last, less));
}

int main () 
{
  printf ("Results of alg_perm02_2:\n");

  int A1[] = { 3, 2, 1 };
  int A2[] = { -3, -2, -1 };
  const int N = sizeof(A1) / sizeof(int);

  all_permutations ("just_less:", A1, A1+N, just_less);
  all_permutations ("abs_less:", A2, A2+N, abs_less);

  return 0;
}

