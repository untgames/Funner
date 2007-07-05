#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec06_test:\n");
  
  int array [] = { 1, 4, 9, 16, 25, 36 };

  vector<int> v (array, array + 6);
  
  int i;
  
  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n"); 

  v.erase (v.begin ()); // Erase first element.

  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n");  
  
  v.erase (v.end () - 1); // Erase last element.
  
  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n");    
  
  v.erase (v.begin() + 1, v.end() - 1); // Erase all but first and last.
  
  for (i=0;i<(int)v.size();i++) printf ("v[%d] = %d\n",i,v [i]);
  printf ("\n");      
  
  return 0;
}
