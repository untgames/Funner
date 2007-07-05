#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of merge0_test:\n");
  
  int numbers1 [5] = {1,6,13,25,101}, numbers2 [5] = {-5,26,36,46,99}, result [10];

  merge (numbers1,numbers1+5,numbers2,numbers2+5,result);
  
  for (int i=0;i<10;i++)
    printf ("%d ",result [i]);

  printf ("\n");  
    
  return 0;
}
