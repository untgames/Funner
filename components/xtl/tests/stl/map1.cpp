#include <stl/map>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of map1_test:\n");

  typedef map<char,int> maptype;

  maptype m;

  // Store mappings between roman numerals and decimals.
  m ['l'] = 50;
  m ['x'] = 20;
  m ['v'] = 5;
  m ['i'] = 1;

  printf ("m['x'] = %d\n",m ['x']);

  m ['x'] = 10;

  printf ("m['x'] = %d\n",m ['x']);
  printf ("m['z'] = %d\n",m ['z']);
  printf ("m.count('z') = %lu\n",m.count ('z'));

  pair<maptype::iterator,bool> p = m.insert (make_pair ('c',100));

  if (p.second) printf ("First insertion successful\n");

  p = m.insert (make_pair ('c',100));

  if (p.second) printf ("Second insertion successful\n");
  else          printf ("Existing pair %c -> %d\n",p.first->first,p.first->second);

  return 0;
}
