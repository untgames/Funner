#include <stl/set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mset4_test:\n");
  
  typedef multiset<int> mset;  
  
  int array [] = {3,6,1,2,3,2,6,7,9};

  mset s (array,array+9);
  
  pair<mset::const_iterator,mset::const_iterator> p = s.equal_range (3);
  
  printf ("lower bound = %d\n",*p.first);
  printf ("upper bound = %d\n",*p.second);  
  
  return 0;
}
