#include "test.h"

#define PRINT(X) printf ("expression '%s'=%lu\n", #X, X)

struct A {};
struct B : A {};
struct C : B {};

int main ()
{
  printf ("Results of tuple2_test:\n");

  PRINT (sizeof (tuple<int, float, const char*>));
  PRINT (sizeof (tuple<A, B, C, A>));
  PRINT (sizeof (tuple<>));

  return 0;
}
