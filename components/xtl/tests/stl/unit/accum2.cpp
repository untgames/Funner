#include <stl/vector>
#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of accum2_test:\n");

  vector <int> v (5);
  
  for (int i=0;i<(int)v.size();i++) v [i] = i+1;
  
  printf ("prod = %d\n",accumulate (v.begin (),v.end (),1,multiplies<int> ()));
  
  return 0;
}
