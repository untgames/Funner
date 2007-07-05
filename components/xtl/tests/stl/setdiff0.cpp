#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of setdiff0_test:\n");
  
  int v1 [3] = {13,18,23}, v2 [4] = {10,13,17,23}, result [4] = {0,0,0,0};

  set_difference ((int*)v1,(int*)v1+3,(int*)v2,(int*)v2+4,(int*)result);
  
  int i;
  
  for(i=0;i<4;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");    
  
  set_difference ((int*)v2,(int*)v2+4,(int*)v1,(int*)v1+2,(int*)result);

  for(i=0;i<4;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");        

  return 0;
}
