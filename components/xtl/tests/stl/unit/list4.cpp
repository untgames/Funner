#include <stl/list>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of list4_test:\n");
  
  int array1 [] = {1,3,6,7}, array2 [] = {2,4};

  list<int> l1 (array1,array1+4), l2 (array2,array2+2);
  
  l1.merge (l2);
  
  for (list<int>::iterator i=l1.begin();i!=l1.end();i++) printf ("%d",*i);
  printf ("\n");
  
  return 0;
}
