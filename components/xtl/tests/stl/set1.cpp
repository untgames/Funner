#include <stl/set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of set1_test:\n");

  set<int> s;

  printf   ("count(42) = %lu\n",s.count (42));
  s.insert (42);
  printf   ("count(42) = %lu\n",s.count (42));
  s.insert (42);
  printf   ("count(42) = %lu\n",s.count (42));
  printf   ("%lu elements erased\n",s.erase (42));

  return 0;
}
