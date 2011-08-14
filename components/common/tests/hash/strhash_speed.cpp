#include <cmath>
#include <cstdio>

#include <common/hash.h>
#include <common/time.h>

using namespace common;

const size_t TEST_SIZE           = 1024 * 1024 * 1024;
const size_t TEST_BUFFER_SIZE    = 1024 + 3;
const size_t TEST_ARRAY_SIZES [] = { 1, 4, 16, 35, 64, 256, 1024 };
const size_t TESTS_COUNT         = sizeof (TEST_ARRAY_SIZES) / sizeof (*TEST_ARRAY_SIZES);
const size_t CORRECT_HASHES []   = { 3431280286, 280021180, 1742029617, 2281313115, 2057480750, 421823093, 3922288482 };

int main ()
{
  printf ("Results of strhash_speed_test:\n");

  unsigned char* test_array = new unsigned char [TEST_BUFFER_SIZE];

  for (size_t i = 0; i < TESTS_COUNT; i++)
  {
    size_t block_size  = TEST_ARRAY_SIZES [i],
           loops_count = (size_t)ceil (TEST_SIZE / (float)block_size);

    unsigned char* data = test_array;

    for (size_t j = 0; j < TEST_BUFFER_SIZE; j++, data++)
      *data = 0;

    data = test_array;

    for (size_t j = 0; j <= block_size; j++, data++)
      *data = j % 254 + 1;

    size_t start_time = common::milliseconds ();

    size_t crc_value = 0;

    for (size_t j = 0; j < loops_count; j++)
      crc_value = strhash ((const char*)test_array, crc_value);

    size_t end_time = common::milliseconds ();

    printf ("Data hash correct %c, crc speed for block size %u = %f MB/s\n", crc_value == CORRECT_HASHES [i] ? 'y' : 'n', block_size, block_size * loops_count / ((end_time - start_time) / 1000.f) / 1024 / 1024);
  }

  delete [] test_array;

  return 0;
}
