#include <cstdio>

#include <common/hash.h>

using namespace common;

const size_t WCHAR_TEST_ARRAY_SIZE = 1000;

int main ()
{
  printf ("Results of strhash_1_test:\n");

  wchar_t* test_array = new wchar_t [WCHAR_TEST_ARRAY_SIZE];

  wchar_t* data = test_array;

  for (size_t i = 0; i < WCHAR_TEST_ARRAY_SIZE; i++, data++)
    *data = 0;

  data = test_array;

  for (size_t i = 0; i < WCHAR_TEST_ARRAY_SIZE; i++, data++)
  {
    printf ("Test %u symbols string:\n", i);

    printf ("  strhash is %x\n", strhash ((const wchar_t*)test_array));
    printf ("  strihash is %x\n", strihash ((const wchar_t*)test_array));
    printf ("  strnhash is %x\n", strnhash ((const wchar_t*)test_array, i));
    printf ("  strnihash is %x\n", strnihash ((const wchar_t*)test_array, i));

    *data = (wchar_t)(WCHAR_TEST_ARRAY_SIZE - i);
  }

  delete [] test_array;

  return 0;
}
