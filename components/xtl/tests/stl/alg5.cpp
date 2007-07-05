#include <stl/deque>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of alg5_test:\n");
  
  deque<int> years;
  
  years.push_back (1962);
  years.push_back (1992);
  years.push_back (2001);
  years.push_back (1999);
    
  for (deque<int>::iterator i=years.begin ();i!=years.end ();i++)
    printf ("%d\n",*i);

  return 0;
}
