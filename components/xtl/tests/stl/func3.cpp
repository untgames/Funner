#include <stl/vector>
#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of func3_test:\n");
  
  vector<int> v;
  
  v.push_back (4);
  v.push_back (1);
  v.push_back (5);
  
  sort (v.begin (),v.end (),greater<int> ());
  
  for (vector<int>::iterator i=v.begin ();i!=v.end ();i++)
    printf ("%d\n",*i);
    
  return 0;
}
