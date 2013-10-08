#include <cstdio>
#include <exception>

#include <common/log.h>
#include <common/time.h>

#include <media/sound.h>

using namespace media;

const char* FILE_NAMES [] = {
  "data/benchmark.wav",
  "data/benchmark_q2.ogg",
  "data/benchmark_q8.ogg",
  "data/benchmark_level_1.flac",
  "data/benchmark_level_7.flac"
};

const size_t OPEN_FILE_COUNT   = 5000;
const size_t DECODE_FILE_COUNT = 4;

void log_print (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
}

int main ()
{
  common::LogFilter log_filter ("media.sound.*", &log_print);

  try
  {
    size_t wav_decoding_time = 0;

	for (size_t i = 0, count = sizeof (FILE_NAMES) / sizeof (*FILE_NAMES); i < count; i++)
	{
	  const char* file_name = FILE_NAMES [i];

	  printf ("Testing file '%s':\n", file_name);

	  //test first file open speed
	  size_t start_time = common::milliseconds ();

	  {
        SoundSample sample (file_name);
	  }

	  size_t test_time = common::milliseconds () - start_time;

	  printf ("  new sound sample first open = %ums\n", test_time);

	  //test open file multiple times
      start_time = common::milliseconds ();

      for (size_t j = 0; j < OPEN_FILE_COUNT; j++)
      {
        SoundSample sample (file_name);
      }

      test_time = common::milliseconds () - start_time;

      printf ("  new sound sample per second = %.0f\n", OPEN_FILE_COUNT / (test_time / 1000.f));

	  //test file decode speed
      SoundSample sample (file_name);

      double total_duration = sample.Duration () * DECODE_FILE_COUNT;
      size_t total_samples  = sample.SamplesCount (),
             buffer_samples = sample.SecondsToSamples (10),
             buffer_size    = sample.SamplesToBytes (1) * buffer_samples;
      char   *data_buffer   = new char [buffer_size];

	  start_time = common::milliseconds ();

      for (size_t j = 0; j < DECODE_FILE_COUNT; j++)
      {
        for (size_t k = 0; k < total_samples;)
          k += sample.Read (k, buffer_samples, data_buffer);
      }

      test_time = common::milliseconds () - start_time;

      if (!i)
        wav_decoding_time = test_time;

      printf ("  decoding speed = %.2fx (%g times slower then wav decoding)\n", total_duration / (test_time / 1000.f), test_time / (double)wav_decoding_time);

      delete [] data_buffer;
	}
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
