#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of alg4_test:\n");
  
  vector<int> years;
  
  years.push_back (1962);
  years.push_back (1992);
  years.push_back (2001);
  years.push_back (1999);
  
  sort (years.begin (),years.end ());
    
  for (vector<int>::const_iterator i=years.begin();i!=years.end ();i++)
    printf ("%d\n",*i);

  return 0;
}
