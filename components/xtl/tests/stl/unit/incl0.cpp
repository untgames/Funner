#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of incl0_test:\n");
  
  int numbers1 [5] = {1,2,3,4,5}, numbers2 [5] = {1,2,4,8,16}, numbers3 [2] = {4,8};

  if (includes (numbers1,numbers1+5,numbers3,numbers3+2)) 
    printf ("numbers1 includes numbers3\n");
  else 
    printf ("numbers1 does not include numbers3\n");
    
  if (includes (numbers2,numbers2+5,numbers3,numbers3+2))
    printf ("numbers2 includes numbers3\n");
  else
    printf ("numbers2 does not include numbers3\n");
    
  return 0;
}
