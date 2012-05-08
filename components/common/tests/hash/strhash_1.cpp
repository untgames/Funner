#include <cstdio>

#include <common/hash.h>

using namespace common;

const size_t CHAR_TEST_ARRAY_SIZE = 0x7F;

int main ()
{
  printf ("Results of strhash_1_test:\n");

  unsigned char* test_array = new unsigned char [CHAR_TEST_ARRAY_SIZE];

  unsigned char* data = test_array;

  for (size_t i = 0; i < CHAR_TEST_ARRAY_SIZE; i++, data++)
    *data = 0;

  data = test_array;

  for (size_t i = 0; i < CHAR_TEST_ARRAY_SIZE; i++, data++)
  {
    printf ("Test %u symbols string:\n", i);

    printf ("  strhash is %x\n", strhash ((const char*)test_array));
    printf ("  strihash is %x\n", strihash ((const char*)test_array));
    printf ("  strnhash is %x\n", strnhash ((const char*)test_array, i));
    printf ("  strnihash is %x\n", strnihash ((const char*)test_array, i));

    *data = CHAR_TEST_ARRAY_SIZE - i;
  }

  delete [] test_array;

  return 0;
}
