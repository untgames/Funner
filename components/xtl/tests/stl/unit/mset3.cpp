#include <stl/set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mset3_test:\n");
  
  int array [] = {3,6,1,2,3,2,6,7,9};

  multiset<int> s (array,array+9); 
  
  printf ("lower bound = %d\n",*s.lower_bound (3));
  printf ("upper bound = %d\n",*s.upper_bound (3));  

  return 0;
}
