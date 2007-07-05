#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec07_test:\n");
  int array1 [] = { 1, 4, 25 }, array2 [] = { 9, 16 };

  vector<int> v (array1,array1+3);
  
  v.insert (v.begin(),0); // Insert before first element.
  v.insert (v.end(),36); // Insert after last element.
  
  int i;
  
  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n");   
  
  // Insert contents of array2 before fourth element.
  v.insert (v.begin() + 3,array2,array2 + 2);

  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n");       
  
  return 0;
}
