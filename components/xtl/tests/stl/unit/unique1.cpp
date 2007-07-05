#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of unique1_test:\n");
  
  int numbers[8] = {0,1,1,2,2,2,3,4};

  unique ((int*)numbers,(int*)numbers+8);
  
  for (int i=0;i<8;i++)
    printf ("%d ",numbers [i]);

  printf ("\n");
  
  return 0;
}
