#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of greateq_test:\n");
  
  int array [4] = {3,1,4,2};

  sort (array,array+4,greater_equal<int> ());
  
  for (int i=0;i<4;i++)
    printf ("%d\n",array [i]);

  return 0;
}
