#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of remove1_test:\n");
  
  int numbers [6] = {1,2,3,1,2,3};

  remove ((int*)numbers,(int*)numbers+6,1);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);

  printf ("\n");  
    
  return 0;
}
