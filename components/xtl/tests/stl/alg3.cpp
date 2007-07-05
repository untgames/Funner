#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of alg3_test:\n");
  
  vector<int> i;
  
  i.push_back (1);
  i.push_back (4);
  i.push_back (2);
  i.push_back (8);
  i.push_back (2);
  i.push_back (2); 
  
  printf ("Count of 2s = %d\n",(int)count (i.begin (),i.end (),2));
  
  return 0;
}
