#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

static bool less_10 (int x)
{
  return x < 10;
}

int main ()
{
  printf ("Results of stblptn0_test:\n");

  int numbers [6] = {10,5,11,20,6,-2};

  stable_partition ((int*)numbers,(int*)numbers+6,less_10);
  
  for (int i=0;i<6;i++) printf ("%d ",numbers [i]);
  printf ("\n");

  return 0;
}
