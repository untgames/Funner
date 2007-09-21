#include <stl/vector>
#include <stl/algorithm>
#include <stl/iterator>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of binsert1_test:\n");
  
  char* array [] = {"laurie","jennifer","leisa"};

  vector<char*> names;
  
  copy (array,array+3,back_insert_iterator<vector<char*> > (names));
  
  for (vector<char*>::iterator i=names.begin();i!=names.end ();i++)
    printf ("%s\n",*i);

  return 0;
}
