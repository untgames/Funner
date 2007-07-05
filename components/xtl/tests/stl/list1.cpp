#include <stl/list>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of list1_test:\n");

  int array1 [] = {9,16,36}, array2 [] = {1,4};

  list<int> l1 (array1,array1+3), l2 (array2,array2+2); 
    
  l1.splice (l1.begin (),l2);    
  
  for (list<int>::iterator i=l1.begin ();i!=l1.end();++i)
    printf ("%d\n",*i);

  return 0;
}
