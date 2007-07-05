#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of reviter1_test:\n");
  
  int array [] = {1,5,2,3};

  vector<int> v (array,array+4);
  
  typedef vector<int>::reverse_iterator reviter;
  
  reviter r (v.end());
  
  while (!(r==v.rbegin ()))
    printf ("%d\n",*r++);

  return 0;
}
