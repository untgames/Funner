#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of iter4_test:\n");
  
  vector<int> v;
  
  v.push_back (1);
  v.push_back (2);
  v.push_back (3);

  vector<int>::iterator i = v.end (); 

  printf ("last element is %d\n",*--i);
  
  i -= 2;
  
  printf ("first element is %d\n",*i);
  
  return 0;
}
