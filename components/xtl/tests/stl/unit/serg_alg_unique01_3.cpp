//Тестирование алгоритма unique_copy (first,last,result)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_unique01_3:\n");

  int A[] = { 5, -5, 5, -5, 5, -5, 5, -5, 4, 4, 4, 4, 7 };
  const int N = sizeof (A) / sizeof (int);
  vector <int> v1 (N);
  
  printf ("Before unique_copy:\n");
  print ("A =", A, A+N);
  print ("v1 =", v1.begin (), v1.end ());

  vector<int>::iterator new_end = unique_copy (A, A+N, v1.begin ());

  printf ("After unique_copy:\n");
  print ("A =", A, A+N);
  print ("v1 =", v1.begin (), new_end);

  return 0;
}

