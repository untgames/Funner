#include <cstdio>

#include <common/hash.h>
#include <common/time.h>

using namespace common;

const size_t LOOPS_COUNT     = 1024 * 8;
const size_t TEST_ARRAY_SIZE = 1024 * 1024 + 3;
const size_t CORRECT_HASH    = 1984837353;

int main ()
{
  printf ("Results of crc32_speed_test:\n");

  unsigned char* test_array = new unsigned char [TEST_ARRAY_SIZE];

  unsigned char* data = test_array;

  for (size_t i = 0; i < TEST_ARRAY_SIZE; i++, data++)
    *data = i % 255;

  size_t start_time = common::milliseconds ();

  size_t crc_value = 0;

  for (size_t i = 0; i < LOOPS_COUNT; i++)
    crc_value = crc32 (test_array, TEST_ARRAY_SIZE, crc_value);

  size_t end_time = common::milliseconds ();

  printf ("Data hash correct %c, crc speed = %f MB/s\n", crc_value == CORRECT_HASH ? 'y' : 'n', TEST_ARRAY_SIZE / ((end_time - start_time) / 1000.f) / 1024 / 1024 * LOOPS_COUNT);

  delete [] test_array;

  return 0;
}
