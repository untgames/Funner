#include "test.h"

int mul_ints(int x, int y) { return x * y; }

struct int_div
{ 
  int operator()(int x, int y) const { return x / y; }; 
};

int main()
{
    printf ("Results of function_arith_cxx98_test:\n");

    function<float (int x, int y)> f;
    
    f = int_div();
    
    if (f) 
    {
      TEST (f (5, 3) == 1);
    }
    else printf ("f has no target, so it is unsafe to call\n");
    
    f = detail::null_type ();
    f = &mul_ints;
    
    TEST (f (5, 3) == 15);
  
    return 0;
}
