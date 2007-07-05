#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of swap1_test:\n");
    
  int a = 42, b = 19;
  
  printf ("a = %d b = %d\n",a,b);
  swap   (a,b);
  printf ("a = %d b = %d\n",a,b);  

  return 0;
}
