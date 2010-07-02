//Тестирование алгоритма uninitialized_fill (first, last, val)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

class Int
{
  public:
    Int (int x) : val (x) {}
    operator int  () const { return val; }
//    operator int  () const { return val; }
  private:
    int val;
};

int main () 
{
  printf ("Results of alg_fill02_1:\n");

  const int N = 4;
  Int val (N);
  Int* A = (Int*) malloc (N * sizeof (Int));

  uninitialized_fill (A, A+N, val);

  print ("A =", A, A+N);

  free (A);

  return 0;
}

