#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of replcpy1_test:\n");
  
  int numbers [6] = {0,1,2,0,1,2}, result [6] = {0,0,0,0,0,0};

  replace_copy ((int*)numbers,(int*)numbers+6,(int*)result,2,42);
  
  for (int i=0;i<6;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");  

  return 0;
}
