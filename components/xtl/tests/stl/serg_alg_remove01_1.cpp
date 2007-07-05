//Тестирование алгоритма remove_copy (first, last, result, value)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_alg_remove01_1:\n");

  int A[] = { 7, 1, 9, 2, 0, 7, 7, 3, 4, 6, 8, 5, 7, 7 };
  const int N = sizeof (A) / sizeof (int);
  vector <int> v1 (N);
  
  printf ("Before remove_copy:\n");
  print ("A  =", A, A+N);
  print ("v1 =", v1.begin (), v1.end ());

  vector<int>::iterator new_end = remove_copy (A, A+N, v1.begin (), 7);

  printf ("After remove_copy:\n");
  print ("A  =", A, A+N);
  print ("v1 =", v1.begin (), new_end);

  return 0;
}

