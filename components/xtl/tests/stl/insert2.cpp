#include <stl/deque>
#include <stl/algorithm>
#include <stl/iterator>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of insert2_test:\n");
  
  char *array1 [] = {"laurie","jennifer","leisa"}, *array2 [] = {"amanda","saskia","carrie"};

  deque<char*> names (array1,array1+3);
   
  copy (array2,array2+3,inserter (names,names.begin ()+2));
  
  for (deque<char*>::iterator j=names.begin ();j!=names.end ();j++)
    printf ("%s\n",*j);

  return 0;
}
