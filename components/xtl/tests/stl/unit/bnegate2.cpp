#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bnegate2_test:\n");
  
  int array [4] = {4,9,7,1};

  sort (array,array+4,not2 (greater<int> ()));
  
  for (int i=0;i<4;i++)
    printf ("%d\n",array [i]);
    
  return 0;
}
