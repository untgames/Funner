#include <stl/bitset>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bitset2_test:\n");

  const bitset<6> b = 0x1111;

  for (size_t i=0; i<b.digits; i++)
    printf ("b [%lu]=%s\n", i, b [i] ? "true" : "false");

  return 0;
}

