#include <stl/algorithm>
#include <stl/vector>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of replif1_test:\n");

  vector<int> v1 (10);
  
  int i;
  
  for (i=0;i<(int)v1.size ();i++)
  {
    v1 [i] = i % 5;
    printf ("%d ",v1 [i]);
  }
  
  printf ("\n");

  replace_if (v1.begin(),v1.end(),odd,42);
  
  for (i=0;i<(int)v1.size();i++)
    printf ("%d ",v1 [i]);

  printf ("\n");
    
  return 0;
}
