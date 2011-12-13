#include <cmath>
#include <cstdio>

#include <common/hash.h>
#include <common/time.h>

using namespace common;

const size_t TEST_SIZE           = 1024 * 1024 * 1024;
const size_t TEST_BUFFER_SIZE    = 1024 * 1024 + 3;
const size_t TEST_ARRAY_SIZES [] = { 4, 16, 35, 64, 256, 1024, 1024 * 16, 1024 * 256, TEST_BUFFER_SIZE };
const size_t TESTS_COUNT         = sizeof (TEST_ARRAY_SIZES) / sizeof (*TEST_ARRAY_SIZES);
const size_t CORRECT_HASHES []   = { 2842971831u, 871311357u, 3201633541u, 2382920724u, 2258954437u, 3615731820u, 3751447993u, 1430708165u, 2975383147u };

int main ()
{
  printf ("Results of crc32_speed_test:\n");

  unsigned char* test_array = new unsigned char [TEST_BUFFER_SIZE];

  unsigned char* data = test_array;

  for (size_t i = 0; i < TEST_BUFFER_SIZE; i++, data++)
    *data = i % 255;

  for (size_t i = 0; i < TESTS_COUNT; i++)
  {
    size_t block_size  = TEST_ARRAY_SIZES [i],
           loops_count = (size_t)ceil (TEST_SIZE / (float)block_size);

    size_t start_time = common::milliseconds ();

    size_t crc_value = 0;

    for (size_t j = 0; j < loops_count; j++)
      crc_value = crc32 (test_array, block_size, crc_value);

    size_t end_time = common::milliseconds ();

    printf ("Data hash correct %c, crc speed for block size %u = %f MB/s\n", crc_value == CORRECT_HASHES [i] ? 'y' : 'n', block_size, block_size * loops_count / ((end_time - start_time) / 1000.f) / 1024 / 1024);
  }

  delete [] test_array;

  return 0;
}
