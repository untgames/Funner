#include <stl/deque>
#include <stl/algorithm>
#include <stl/iterator>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of finsert2_test:\n");
  
  const char* array [] = {"laurie","jennifer","leisa"};

  deque<const char*> names;
  
  copy (array,array+3,front_inserter (names));
    
  for (deque<const char*>::iterator i=names.begin ();i!=names.end ();i++)
    printf ("%s\n",*i);

  return 0;
}
