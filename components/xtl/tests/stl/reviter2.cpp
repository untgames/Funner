#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of reviter2_test:\n");
  
  int array [] = {1,5,2,3};

  vector<int> v (array,array+4);  
  
  for (vector<int>::reverse_iterator r=v.rbegin ();r!=v.rend();r++)
    printf ("%d\n",*r);

  return 0;
}
