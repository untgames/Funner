// Тестирование splice
#include <stdio.h>
#include <stl/list>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of list11_test:\n");

  list<int> c1, c2, c3, c4;
  
  c1.push_back (10);
  c1.push_back (11);
  c2.push_back (12);
  c2.push_back (20);
  c2.push_back (21);
  c3.push_back (30);
  c3.push_back (31);
  c4.push_back (40);
  c4.push_back (41);
  c4.push_back (42);

  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());
  print ("c4 =", c4.begin (), c4.end ());

  c2.splice (++c2.begin (), c1);
  printf ("After splicing c1 into c2:\n");
  print ("c1 =", c1.begin (), c1.end ());
  print ("c2 =", c2.begin (), c2.end ());

  c2.splice (++c2.begin (), c3, c3.begin ());
  printf ("After splicing the first element of c3 into c2:\n");
  print ("c2 =", c2.begin (), c2.end ());
  print ("c3 =", c3.begin (), c3.end ());

  c2.splice (++c2.begin (), c4, c4.begin (), --c4.end ());
  printf ("After splicing a range of c4 into c2:\n");
  print ("c2 =", c2.begin (), c2.end ());
  print ("c4 =", c4.begin (), c4.end ());

  return 0;
}
