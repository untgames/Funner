#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec05_test:\n");
  
  int array [] = { 1, 4, 9, 16 };

  vector<int> v (array,array+4);  
  
  for (int i=0;i<(int)v.size ();i++)
    printf ("v[%d] = %d\n",i,v [i]);

  return 0;
}
