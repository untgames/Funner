#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int less_10 (int a)
{
  return a < 10 ? 1 : 0;
}

int main ()
{
  printf ("Results of ptition0_test:\n");
  
  int numbers [6] = {6,12,3,10,1,20};

  partition (numbers,numbers+6,less_10);
  
  for (int i=0;i<6;i++)
    printf ("%d ",numbers [i]);
    
  printf ("\n");  
    
  return 0;  
}
