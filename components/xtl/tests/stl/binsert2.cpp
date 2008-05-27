#include <stl/vector>
#include <stl/algorithm>
#include <stl/iterator>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of binsert2_test:\n");
  
  const char* array [] = {"laurie","jennifer","leisa"};

  vector<const char*> names;
  
  copy (array,array+3,back_inserter (names));

  for (vector<const char*>::iterator i=names.begin ();i!=names.end ();i++)
    printf ("%s\n",*i);
    
  return 0;
}
