#include "shared.h"

void test (size_t w, size_t h, size_t d)
{
  printf ("get_mips_count(%u, %u, %u)=%u\n", w, h, d, get_mips_count (w, h, d));
}

int main ()
{
  printf ("Results of utils4_test:\n");

  test (1024, 1, 1);  
  test (1024, 500, 1);  
  test (1024, 500, 2049);
  test (4096, 4096, 1);
  test (8192, 8192, 1);
  test (8192 * 2, 8192 * 2, 1);

  return 0;
}

