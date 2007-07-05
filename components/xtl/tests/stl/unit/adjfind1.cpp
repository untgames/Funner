#include <stl/vector>
#include <stl/algorithm>

using namespace stl;

int main ()
{
  printf ("Results of adjfind1_test:\n");
  
  typedef vector<int> IntVector;

  IntVector v(10);
  
  for (int i=0;i<(int)v.size();i++) v [i] = i;
  
  IntVector::iterator location = adjacent_find (v.begin (),v.end ());
  
  if(location != v.end ()) printf ("Found adjacent pair of: %d\n",*location);
  else printf ("No adjacent pairs\n");
  
  v [6] = 7;
  
  location = adjacent_find (v.begin (),v.end ());
  
  if(location != v.end ()) printf ("Found adjacent pair of: %d\n",*location);
  else printf ("No adjacent pairs\n");
  
  return 0;
}
