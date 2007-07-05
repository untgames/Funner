#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bind1st2_test:\n");

  int array [3] = {1,2,3};

  int* p = remove_if ((int*)array,(int*)array+3,bind1st (less<int> (),2));
  
  for (int* i=array;i!=p;i++)
    printf ("%d\n",*i);

  return 0;
}
