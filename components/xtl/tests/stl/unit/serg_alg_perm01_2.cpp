//Тестирование алгоритма next_permutation (first,last,less)
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
  } while (next_permutation (first, last, less));
}

int main () 
{
  printf ("Results of serg_alg_perm01_2:\n");

  int A1[] = { 1, 2, 3 };
  int A2[] = { -1, -2, -3 };
  const int N = sizeof(A1) / sizeof(int);

  all_permutations ("just_less:", A1, A1+N, just_less);
  all_permutations ("abs_less:", A2, A2+N, abs_less);

  return 0;
}

