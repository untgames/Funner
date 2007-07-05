#include <stl/list>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of revbit2_test:\n");
  
  int array [] = {1,5,2,3};  

  list<int> v (array,array+4);
  
  list<int>::reverse_iterator r;
  
  for (r=v.rbegin ();!(r==v.rend());r++) printf ("%d\n",*r);
  
  return 0;
}
