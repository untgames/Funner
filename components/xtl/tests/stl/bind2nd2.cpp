#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bind2nd2_test:\n");
  
  int array [3] = {1,2,3};

  replace_if (array,array+3,bind2nd (greater<int> (),2),4);
  
  for (int i=0;i<3;i++)
    printf ("%d\n",array [i]);

  return 0;
}
