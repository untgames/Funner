#include <stl/hash_map>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of hmap2_test:\n");
  
  typedef hash_map<pair<int, int>, int> my_map;
  
  my_map m;
  
  m [make_pair (1, 2)] = 3;
  m [make_pair (3, 4)] = 5;
  m [make_pair (1, 2)] = 0;
  
  for (my_map::iterator iter=m.begin (); iter!=m.end (); ++iter)
    printf ("(%d, %d) -> %d\n", iter->first.first, iter->first.second, iter->second);

  return 0;
}
