// Тестирование lower_bound ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of set6_test:\n");

  set <int> s1;
  set <int>::const_iterator iter;
  
  s1.insert (10);
  s1.insert (20);
  s1.insert (30);

  print ("s1 =", s1.begin (), s1.end ());

  iter = s1.lower_bound (20);
  printf ("Found element: %d\n", *iter);

  iter = s1.lower_bound (40);

  // If no match is found for the key, end () is returned
  if ( iter == s1.end () )
     printf ("Found 'end ()'\n");
  else
     printf ("Found???: %d\n", *iter);

  iter = s1.lower_bound (*(--s1.end ()));
  printf ("Found element: %d\n", *iter);

  return 0;
}
