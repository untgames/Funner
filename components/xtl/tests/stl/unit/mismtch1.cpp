#include <stl/vector>
#include <stl/algorithm>
#include <stl/numeric>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mismtch1_test:\n");
  
  typedef vector<int> IntVec;
  
  IntVec v1 (10), v2 (v1.size ());
  
  iota (v1.begin (),v1.end (),0);
  iota (v2.begin (),v2.end (),0);
  
  pair<IntVec::iterator,IntVec::iterator> result = mismatch (v1.begin (),v1.end (),v2.begin ());
  
  if (result.first == v1.end () && result.second == v2.end ())
    printf ("v1 and v2 are the same\n");
  else
    printf ("mismatch at index: %d\n",result.first-v1.begin());
    
  v2 [v2.size ()/2] = 42;
  
  result = mismatch (v1.begin (),v1.end (),v2.begin ());
  
  if (result.first == v1.end () && result.second == v2.end ())
    printf ("v1 and v2 are the same\n");
  else
    printf ("mismatch at index: %d\n",result.first-v1.begin());
    
  return 0;
}
