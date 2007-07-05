#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of incl1_test:\n");
  
  vector<int> v1 (10), v2 (3);
  
  int i;
  
  for (i=0;i<(int)v1.size();v1 [i]=i++);
  
  if(includes(v1.begin (),v1.end (),v2.begin (),v2.end ()))
    printf ("v1 includes v2\n");
  else
    printf ("v1 does not include v2\n");
    
  for (i=0;i<(int)v2.size();i++) v2 [i] = i+3;
  
  if (includes (v1.begin (),v1.end (),v2.begin (),v2.end ()))
    printf ("v1 includes v2\n");
  else
    printf ("v1 does not include v2\n");
    
  return 0;
}
