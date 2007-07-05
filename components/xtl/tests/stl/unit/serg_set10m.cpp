// Тестирование insert ()
#include <stdio.h>
#include <stl/set>
#include "test.h"

using namespace stl;

int main () 
{
  printf ("Results of serg_set10m_test:\n");

  multiset <int, less<int> > ms1, ms2;

  ms1.insert (10);
  ms1.insert (20);
  ms1.insert (30);
  ms1.insert (40);

  print ("ms1 =", ms1.begin (), ms1.end ());

  ms1.insert (20);
  ms1.insert (--ms1.end (), 50);

  print ("ms1 =", ms1.begin (), ms1.end ());

  ms2.insert (100);
  ms2.insert (++ms1.begin (), --ms1.end ());

  print ("ms2 =", ms2.begin (), ms2.end ());

  return 0;
}
