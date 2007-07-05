#include <stl/list>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of list2_test:\n");
  
  int array1 [] = {1,16}, array2 [] = {4,9};

  list<int> l1 (array1,array1+2), l2 (array2,array2+2);
    
  list<int>::iterator i = l1.begin();  
  i++;
  
  l1.splice (i,l2,l2.begin (),l2.end ());
  
  for (i=l1.begin ();i!=l1.end();++i)
    printf ("%d\n",*i);

  return 0;
}
