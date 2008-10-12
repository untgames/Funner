#include <stdio.h>
#include <stl/vector>

using namespace stl;

int main ()
{
  vector<int> v;

  printf ("Results of vec01_test:\n");
  printf ("empty = %d\n",v.empty ());
  printf ("size = %lu\n",v.size ());
  printf ("max_size = %lu\n",v.max_size ());

  v.push_back (42);

  printf ("size = %lu\n",v.size ());
  printf ("v1[0] = %d\n",v [0]);

  return 0;
}
