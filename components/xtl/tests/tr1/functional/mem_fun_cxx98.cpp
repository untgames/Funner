#include "test.h"

struct X
{
  int foo(int);
};

int X::foo(int x) { return -x; }

int main()
{
    printf ("Results of mem_fun_cxx98_test:\n");

    function<int (X*, int)> f;

    f = &X::foo;
  
    X x;
    
    TEST (f(&x, 5) == -5);

    return 0;
}
