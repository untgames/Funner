#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of iota1_test:\n");
  
  int numbers [10];
  
  iota (numbers,numbers+10,42);
  
  for (int i=0;i<10;i++)
    printf ("%d ",numbers [i]);
    
  printf ("\n");
  
  return 0;
}
