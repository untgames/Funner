#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of filln1_test:\n");
  
  vector<int> v (10);
  
  fill_n (v.begin (),v.size (),42);
  
  for (int i=0;i<10;i++) printf ("%d ",v [i]);

  printf ("\n");
  
  return 0;
}
