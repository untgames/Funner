#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of advance_test:\n");
  
  typedef vector <int> IntVector;
  
  IntVector v (10);
  
  for (int i=0;i<(int)v.size();i++) v [i] = i;
  
  IntVector::iterator location = v.begin ();

  printf  ("At Beginning: %d\n",*location);  
  advance (location, 5);
  printf  ("At Beginning + 5: %d\n",*location);
  
  return 0;
}
