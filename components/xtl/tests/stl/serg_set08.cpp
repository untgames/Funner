// Тестирование функции equal_range
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_set8_test:\n");

  set <int> s1;
  set <int>::const_iterator iter;
  
  s1.insert (10);
  s1.insert (20);
  s1.insert (30);

  print ("s1 =", s1.begin (), s1.end ());

  pair <set <int>::const_iterator, set <int>::const_iterator> p1, p2;
  p1 = s1.equal_range (20);

  printf ("lower_bound (20): %d\n", *p1.first);
  printf ("upper_bound (20): %d\n", *p1.second);

  iter = s1.lower_bound (20);
  printf ("A direct call of lower_bound (20) gives %d\n", *iter);
  iter = s1.upper_bound (20);
  printf ("A direct call of upper_bound (20) gives %d\n", *iter);

  p2 = s1.equal_range (40);

  if ((p2.first  == s1.end ()) && 
      (p2.second == s1.end ()))
     printf ("Not Found!\n");
  else
     printf ("Found!\n");

  return 0;
}
