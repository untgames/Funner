#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of alg1_test:\n");    
  printf ("min(4, 7) = %d\nmax('a', 'z') = %c\n",min (4,7),max ('a','z'));

  return 0;
}
