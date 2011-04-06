#include "shared.h"

#define TEST(X) printf ("Results of '%s'=%d, ", #X, X); printf ("value=%d\n", value)

int main ()
{
  printf ("Results of interlocked_test:\n");
  
  int value = 1;
  
  TEST (xtl::atomic_conditional_increment (value));
  TEST (xtl::atomic_conditional_decrement (value));  
  TEST (xtl::atomic_conditional_decrement (value));    
  TEST (xtl::atomic_conditional_decrement (value));    
  
  value = 0;
  
  TEST (xtl::atomic_increment (value));
  TEST (xtl::atomic_decrement (value));  

  return 0;
}
