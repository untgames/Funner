// Тестирование конструкторов
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of set1_test:\n");

  set <int> s0;

  set <int, less<int> > s1;
  s1.insert (10);
  s1.insert (20);
  s1.insert (30);
  s1.insert (40);

  set <int, greater<int> > s2;
  s2.insert (10);
  s2.insert (20);

  set <int>::const_iterator first, last;
  first = s1.begin ();
  last = ++s1.begin ();
  set <int> s3 (first, ++last);

  set <int>::allocator_type s2_Alloc = s2_Alloc = s2.get_allocator ();
  set <int> s4 (s3.begin (), ++s3.begin (), less<int>( ), s2_Alloc);

  print ("s0 =", s0.begin (), s0.end ());
  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());
  print ("s3 =", s3.begin (), s3.end ());
  print ("s4 =", s4.begin (), s4.end ());

  return 0;
}
