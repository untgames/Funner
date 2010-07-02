//Тестирование алгоритма transform (first1, last1, result, op)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

template <class T>
class multiply
{
  public:
    multiply (const T& x) : factor (x) {}

    T operator () (T& arg) const 
    {
      return arg * factor;
    }
  private:
    T factor;
};

int main () 
{
  printf ("Results of alg_trans01_1:\n");

  int A[] = { -4, -3, -2, -1, 0, 1, 2 };
  const int N = sizeof (A) / sizeof (N);

  printf ("Before transform:\n");
  print ("A =", A, A+N);

  transform (A, A+N, A, multiply<int>(2));

  printf ("After transform:\n");
  print ("A =", A, A+N);

  return 0;
}

