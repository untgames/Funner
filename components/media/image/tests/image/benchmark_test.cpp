#include "shared.h"

using namespace media;
using namespace common;

const char*  FILES []    = { "data/benchmark.png", "data/benchmark_q50.jpg", "data/benchmark_q100.jpg" };
const size_t LOOPS_COUNT = 50;

int main ()
{
  try
  {
    printf ("Results of benchmark_test:\n");

    for (size_t i = 0, count = sizeof (FILES) / sizeof (*FILES); i < count; i++)
    {
      const char* file_name = FILES [i];

      printf ("Testing file '%s'\n", file_name);

      Image first_load (file_name);

      size_t start_time = milliseconds ();

      for (size_t j = 0; j < LOOPS_COUNT; j++)
        Image image (file_name);

      size_t test_time = milliseconds () - start_time;

      printf ("Decoding speed %fMP/s\n", LOOPS_COUNT * first_load.Width () * first_load.Height () / (float)test_time / 1000.f);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  return 0;
}
