#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of stblsrt1_test:\n");
  
  int array [6] = {1,50,-10,11,42,19};

  stable_sort (array,array+6);
  
  for (int i=0;i<6;i++) printf ("%d ",array [i]);
  printf ("\n");
  
  return 0;
}
