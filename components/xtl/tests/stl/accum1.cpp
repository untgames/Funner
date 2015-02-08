#include <stl/vector>
#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of accum1_test:\n");
  
  vector<int> v (5);
  
  for (size_t i=0;i<v.size();i++) v [i] = (int)i+1;
  
  printf ("sum = %d\n",accumulate (v.begin (),v.end (),0));

  return 0;
}
