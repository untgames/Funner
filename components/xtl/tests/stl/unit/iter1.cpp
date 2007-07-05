#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of iter1_test:\n");

  typedef vector<const char*> vec;    
  
  vec v;
  
  v.push_back ("zippy");
  v.push_back ("motorboy");
   
  for(vec::iterator i=v.begin ();i!=v.end ();i++)
    printf ("%s\n",*i);

  return 0;
}
