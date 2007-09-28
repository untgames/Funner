#include "test.h"

using namespace stl;

struct X
{
  int foo(int);
};

int X::foo(int x) { return -x; }

int main()
{
  printf ("Results of std_bind_cxx98_test:\n");

  function<int (int)> f;
  
  X x;
  
  f = bind1st (mem_fun (&X::foo), &x);
  
  TEST (f (5) == -5);

  return 0;
}
