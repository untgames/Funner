#include <cstdio>

#include <common/hash.h>

using namespace common;

const size_t TEST_ARRAY_SIZE = 64;

int main ()
{
  printf ("Results of crc32_1_test:\n");

  unsigned char* test_array = new unsigned char [TEST_ARRAY_SIZE];

  unsigned char* data = test_array;

  for (size_t i = 0; i < TEST_ARRAY_SIZE; i++, data++)
    *data = i % 255;

  for (size_t i = 0; i <= TEST_ARRAY_SIZE; i++)
    printf ("%u bytes crc is %x\n", i, crc32 (test_array, i));

  delete [] test_array;

  return 0;
}
