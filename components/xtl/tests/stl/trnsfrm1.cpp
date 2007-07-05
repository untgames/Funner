#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline int negate_int (int x)
{
  return -x;
}

int main ()
{
  printf ("Results of trnsfrm1_test:\n");

  int numbers [6] = {-5,-1,0,1,6,11}, result [6];

  transform ((int*)numbers,(int*)numbers+6,result,negate_int);
  
  for (int i=0;i<6;i++) printf ("%d ",result [i]);
  printf ("\n");

  return 0;
}
