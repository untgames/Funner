#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of uniqcpy1_test:\n");
  
  int numbers[8] = {0,1,1,2,2,2,3,4}, result[8] = {0,0,0,0,0,0,0,0};

  unique_copy ((int*)numbers,(int*)numbers+8,(int*)result);
  
  for (int i=0;i<8;i++)
    printf ("%d ",result [i]);

  printf ("\n");  
    
  return 0;
}
