// Тестирование find ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of set4_test:\n");

  set <int> s1;
  set <int>::const_iterator iter;
  
  s1.insert ( 10 );
  s1.insert ( 20 );
  s1.insert ( 30 );

  iter = s1.find (20);
  printf ("Found element: %d\n", *iter);

  iter = s1.find (40);
  if ( iter == s1.end () )
    printf ("Not Found!!!\n");
  else
    printf ("Found???\n");

  iter = s1.find (*(--s1.end ()));
  printf ("Found element: %d\n", *iter);

  return 0;
}
