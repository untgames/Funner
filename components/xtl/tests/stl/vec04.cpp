#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec04_test:\n");
  
  vector<int> v(4);
  
  v [0] = 1;
  v [1] = 4;
  v [2] = 9;
  v [3] = 16;
  
  printf ("front = %d\n",v.front());
  printf ("back = %d, size = %d\n",v.back (),v.size ());

  v.push_back(25);

  printf ("back = %d, size = %d\n",v.back (),v.size ());  
  
  v.pop_back();
  
  printf ("back = %d, size = %d\n",v.back (),v.size ());    
  
  return 0;
}
