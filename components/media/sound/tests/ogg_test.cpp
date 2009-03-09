#include <cstdio>
#include <exception>

#include <common/hash.h>
#include <common/log.h>

#include <media/sound.h>

using namespace media;
using namespace common;

const char* file_name = "data/sound1.ogg";

void log_print (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
}

int main ()
{
  printf ("Results of ogg_test:\n");

  common::LogFilter log_filter ("media.sound.*", &log_print);

  try
  {
    SoundSample sample2 (file_name);
    SoundSample sample1 = sample2;

    SoundSample ().Swap (sample2);

    printf ("SamplesCount:  %u\n", sample1.SamplesCount ());
    printf ("SizeInBytes:   %u\n", sample1.SizeInBytes ());
    printf ("Frequency:     %u\n", sample1.Frequency ());
    printf ("Channels:      %u\n", sample1.Channels ());
    printf ("BitsPerSample: %u\n", sample1.BitsPerSample ());
    printf ("Duration:      %.2f\n", sample1.Duration ());

/*    char* data_buffers[2];
    size_t i = 0;

    size_t buffer_size = sample1.SamplesToBytes (1) * sample1.SamplesCount ();
    data_buffers[0] = new char [buffer_size];
    data_buffers[1] = new char [buffer_size];

    sample1.Read (0, buffer_size, data_buffers[0]);

    size_t one_read = sample1.SamplesCount() / 6 + sample1.SamplesCount() / 15;

    for (; i < sample1.SamplesCount();)
      i += sample1.Read (i, one_read, data_buffers[1] + i * sample1.SamplesToBytes (1));

    unsigned char hash[2][16];

    md5 (hash[0], data_buffers[0], buffer_size);
    md5 (hash[1], data_buffers[1], buffer_size);

    for (size_t j = 0; j < 2; j++)
    {
      printf ("Hash %u: {", j);
      for (size_t i = 0; i < 15; i++)
        printf ("%02x", hash[j][i]);
      printf ("%02x}\n", hash [j][15]);
    }

    delete [] data_buffers [0];
    delete [] data_buffers [1];*/
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
