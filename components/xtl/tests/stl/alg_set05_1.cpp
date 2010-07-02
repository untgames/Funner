//Тестирование алгоритма set_symmetric_difference (range1,range2,result)
#include <stdio.h>
#include <stl/algorithm>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of alg_set05_1:\n");

  int A1[] = {1, 3, 5, 7, 9, 11};
  int A2[] = {1, 1, 2, 3, 5, 8, 13};
  const int N1 = sizeof (A1) / sizeof (int);
  const int N2 = sizeof (A2) / sizeof (int);
  vector <int> v1 (N1+N2);

  print ("A1 =", A1, A1+N1);
  print ("A2 =", A2, A2+N2);

  vector<int>::iterator new_end = set_symmetric_difference (A1, A1 + N1, A2, A2 + N2, v1.begin ());

  printf ("Symmetric difference of A1 and A2:\n");
  print ("v1 =", v1.begin (), new_end);

  return 0;
}

