#include <stl/deque>
#include <stl/algorithm>
#include <stl/iterator>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of finsert2_test:\n");
  
  char* array [] = {"laurie","jennifer","leisa"};

  deque<char*> names;
  
  copy (array,array+3,front_inserter (names));
    
  for (deque<char*>::iterator i=names.begin ();i!=names.end ();i++)
    printf ("%s\n",*i);

  return 0;
}
