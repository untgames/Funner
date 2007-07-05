#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline bool div_3 (int a)
{
  return a % 3 ? 0 : 1;
}

int main ()
{
  printf ("Results of findif1_test:\n");

  typedef vector<int> IntVec;
  
  IntVec v (10);
  
  for (int i=0;i<(int)v.size ();i++) v [i] = (i+1)*(i+1);
  
  IntVec::iterator iter = find_if (v.begin (),v.end (),div_3);
  
  if (iter != v.end ())
    printf ("Value %d at offset %d is divisible by 3\n",*iter,iter-v.begin ());

  return 0;
}
