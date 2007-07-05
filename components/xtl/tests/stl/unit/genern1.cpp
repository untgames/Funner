#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of genern1_test:\n");

  vector<int> v (10);

  generate_n (v.begin (),v.size(),myrand);

  for (int i=0;i<10;i++) printf ("%d ",v [i]);
  printf ("\n");

  return 0;
}
