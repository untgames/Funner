#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of search0_test:\n");
  
  int v1 [6] = {1,1,2,3,5,8}, v2 [6] = {0,1,2,3,4,5}, v3 [2] = {3,4};

  int* location = search (v1,v1+6,v3,v3+2);
  
  if (location == v1+6) printf ("v3 not contained in v1\n");
  else                  printf ("Found v3 in v1 at offset: %d\n",location-v1);
  
  location = search (v2,v2+6,v3,v3+2);
  
  if (location == v2+6) printf ("v3 not contained in v2\n");
  else                  printf ("Found v3 in v2 at offset: %d\n",location-v2);
  
  return 0;
}
