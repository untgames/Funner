#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec08_test:\n");
  
  vector<int> v;
   
  printf ("capacity = %d\n",v.capacity ());

  v.push_back(42); 
  
  printf ("capacity = %d\n",v.capacity ());  

  v.reserve(5000);
  
  printf ("capacity = %d\n",v.capacity ());  
  
  return 0;
}
