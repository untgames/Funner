//Тестирование алгоритма uninitialized_copy_n (first, count, result)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

class Int
{
  public:
    Int (int x) : val (x) {}
    operator int& ()       { return val; }
//    operator int  () const { return val; }
  private:
    int val;
};

int main () 
{
  printf ("Results of serg_alg_copy02_2:\n");

  int A1[] = { 10, 20, 30, 40, 50 };
  const int N = sizeof (A1) / sizeof (int);

  print ("A1 =", A1, A1+N);

  Int* A2 = (Int*) malloc (N * sizeof (Int));
  uninitialized_copy_n (A1, N, A2);

  print ("A2 =", A2, A2+N);

  free (A2);

  return 0;
}

