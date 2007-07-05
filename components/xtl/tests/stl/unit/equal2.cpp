#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

bool values_squared (int a,int b)
{
  return a*a == b;
}

int main ()
{
  printf ("Results of equal2_test:\n");

  vector<int> v1 (10), v2 (10);
  
  for (int i=0;i<(int)v1.size ();i++)
  {
    v1 [i] = i;
    v2 [i] = i*i;
  }

  printf ("v2[i] %s v1[i] * v1[i]\n",equal (v1.begin (),v1.end (),v2.begin (),values_squared)?"==":"!=");
    
  return 0;
}
