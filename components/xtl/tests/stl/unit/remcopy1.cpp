#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of remcopy1_test:\n");
  
  int numbers [6] = {1,2,3,1,2,3}, result[6] = {0,0,0,0,0,0};

  remove_copy ((int*)numbers,(int*)numbers+6,(int*)result,2);
  
  for (int i=0;i<6;i++)
    printf ("%d ",result [i]);
    
  printf ("\n");  
  
  return 0;
}
