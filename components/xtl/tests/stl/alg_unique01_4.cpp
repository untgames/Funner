//Тестирование алгоритма unique_copy (first,last,result,pred)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

inline bool abs_eq (int a, int b) { return abs (a) == abs (b); };

int main () 
{
  printf ("Results of alg_unique01_4:\n");

  int A[] = { 5, -5, 5, -5, 5, -5, 5, -5, 4, 4, 4, 4, 7 };
  const int N = sizeof (A) / sizeof (int);
  vector <int> v1 (N);
  
  printf ("Before unique_copy:\n");
  print ("A =", A, A+N);
  print ("v1 =", v1.begin (), v1.end ());

  vector<int>::iterator new_end = unique_copy (A, A+N, v1.begin (), abs_eq);

  printf ("After unique_copy:\n");
  print ("A =", A, A+N);
  print ("v1 =", v1.begin (), new_end);

  return 0;
}

