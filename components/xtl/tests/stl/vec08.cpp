#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec08_test:\n");

  vector<int> v;

  printf ("capacity = %lu\n",v.capacity ());

  v.push_back(42);

  printf ("capacity = %lu\n",v.capacity ());

  v.reserve(5000);

  printf ("capacity = %lu\n",v.capacity ());

  return 0;
}
