#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bvec1_test:\n");

  bool array [3] = {1,0,1};  
  bit_vector<> v1 (3);
  
  int i;
  
  for (i=0;i<(int)v1.size();i++) printf ("%d",(bool)v1 [i]);
  printf ("\n");  

  v1 [0] = v1 [2] = 1;
  
  for (i=0;i<(int)v1.size();i++) printf ("%d",(bool)v1 [i]);
  printf ("\n");
  
  v1.insert (v1.begin (),array,array+2);
  
  for (i=0;i<(int)v1.size();i++) printf ("%d",(bool)v1 [i]);
  printf ("\n");
  
  bit_vector<> v2 = v1;
  
  if (v1 != v2)
    printf ("v1 != v2\n");

  v1 [0] |= 0;
  v1 [1] |= 0;
  v1 [2] |= 1;
  v1 [3] |= 1;
  
  for (i=0;i<(int)v1.size();i++) printf ("%d",(bool)v1 [i]);
  printf ("\n"); 
  
  v2 [0] &= 0;
  v2 [1] &= 0;
  v2 [2] &= 1;
  v2 [3] &= 1;
  
  for (i=0;i<(int)v2.size();i++) printf ("%d",(bool)v2 [i]);
  printf ("\n");
  
  return 0;
}
