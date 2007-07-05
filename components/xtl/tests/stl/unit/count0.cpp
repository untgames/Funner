#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of count0_test:\n");
  
  int numbers [10] = {1,2,4,1,2,4,1,2,4,1}, result = count (numbers,numbers+10,1);

  printf ("Found %d 1's.\n",result);
  
  return 0;
}
