#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of setintr0_test:\n");

  int v1 [3] = {13,18,23}, v2 [4] = {10,13,17,23}, result [4] = {0,0,0,0};

  set_intersection ((int*)v1,(int*)v1+3,(int*)v2,(int*)v2+4,(int*)result);
  
  for (int i=0;i<4;i++)
    printf ("%d ",result [i]);
  
  printf ("\n");
    
  return 0;
}
