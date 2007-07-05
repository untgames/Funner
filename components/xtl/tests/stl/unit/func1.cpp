#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline bool bigger (int i)
{
  return i > 3;
}

int main ()
{
  printf ("Results of func1_test:\n");

  vector<int>v;
  
  v.push_back (4);
  v.push_back (1);
  v.push_back (5);
  
  int n = 0;
  
  count_if (v.begin (),v.end (),bigger,n);
  
  printf ("Number greater than 3 = %d\n",n);
  
  return 0;
}
