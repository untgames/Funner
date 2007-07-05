#include <stl/set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of set2_test:\n");
  
  set<int> s;
  
  pair<set<int>::const_iterator,bool> p = s.insert (42);
  
  printf (p.second?"Inserted new element %d\n":"Existing element = %d\n",*p.first);

  p = s.insert (42);
  
  printf (p.second?"Inserted new element %d\n":"Existing element = %d\n",*p.first);  
  
  return 0;
}
