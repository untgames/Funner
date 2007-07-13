#include "test.h"

struct stateful_type { int operator()(int) const { return 0; } };

int main()
{
  printf ("Results of function_ref_cxx98_test:\n");

  stateful_type a_function_object;
  
  function<int (int)> f;
  
  f = ref (a_function_object);

  function<int (int)> f2(f);
  
  TEST (f (1) == 0);
  TEST (f2 (1) == 0);  

  return 0;
}
