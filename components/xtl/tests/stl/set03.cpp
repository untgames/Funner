// Тестирование функций удаления: erase (), clear ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of set3_test:\n");

  set <int, less<int> > s1, s2, s3;
  set <int, less<int> >::size_type n;
  int i;

  for (i=1; i<5; i++)
  {
    s1.insert(i);
    s2.insert(i * i);
    s3.insert(i - 1);
  }
  printf ("The initial sets is:\n");
  print ("s1 =", s1.begin (), s1.end ());
  print ("s2 =", s2.begin (), s2.end ());
  print ("s3 =", s3.begin (), s3.end ());

  s1.erase (++s1.begin());

  printf ("After the 2nd element is deleted, the set s1 is:\n");
  print ("s1 =", s1.begin (), s1.end ());

  s2.erase (++s2.begin(), --s2.end());

  printf ("After the middle two elements are deleted, the set s2 is:\n");
  print ("s2 =", s2.begin (), s2.end ());

  n = s3.erase (2);

  printf ("After the element with a key of 2 is deleted, the set s3 is:\n");
  print ("s3 =", s3.begin (), s3.end ());

  printf ("The number of elements removed from s3 is: %lu\n", n);

  printf ("The size of the s3 is %lu\n", s3.size ());
  s3.clear ();
  printf ("The size of s3 after clearing is %lu\n", s3.size ());

  return 0;
}
