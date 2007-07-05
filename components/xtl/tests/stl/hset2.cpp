#include <stl/algorithm>
#include <stl/hash_set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of hset2_test:\n");  
  
  hash_set<int> s;
  
  pair<hash_set<int>::const_iterator,bool> p = s.insert (42);
  
  printf (p.second?"Inserted new element %d\n":"Existing element = %d\n",*p.first);

  p = s.insert(42);
  
  printf (p.second?"Inserted new element %d\n":"Existing element = %d\n",*p.first);    

  return 0;
}
