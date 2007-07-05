#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline bool bigger_than (int x,int y)
{
  return x > y;
}

int main ()
{
  printf ("Results of func2_test:\n");

  vector<int> v;
  
  v.push_back (4);
  v.push_back (1);
  v.push_back (5);
  
  sort (v.begin (),v.end (),bigger_than);
    
  for (vector<int>::iterator i=v.begin ();i!=v.end ();i++)
    printf ("%d\n",*i);
    
  return 0;
}
