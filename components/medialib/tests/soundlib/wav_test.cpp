#include <windows.h>
#include <stdio.h>
#include <common/exception.h>
#include <common/hash.h>
#include <media/sound.h>
#include <memory.h>

using namespace medialib;
using namespace common;

const char* file_name = "data/sound2.wav";

int main ()
{
  try
  {
    size_t i = 0;
    unsigned char hash[2][16];

    SoundSample* sample2 = new SoundSample (file_name);
    SoundSample sample1 (*sample2);
    delete sample2;

    char* data_buffers[2];

    size_t buffer_size = sample1.SamplesToBytes (1) * sample1.SamplesCount ();
    data_buffers[0] = new char [buffer_size];
    data_buffers[1] = new char [buffer_size];

    sample1.Read (0, sample1.SamplesCount(), data_buffers[0]);

    size_t one_read = sample1.SamplesCount() / 6 + sample1.SamplesCount() / 15;

    for (; i < sample1.SamplesCount();)
      i += sample1.Read (i, one_read, data_buffers[1] + i * sample1.SamplesToBytes (1));

    md5 (hash[0], data_buffers[0], buffer_size);
    md5 (hash[1], data_buffers[1], buffer_size);

    printf ("Results of wav_test:\n");

    for (size_t j = 0; j < 2; j++)
    {
      printf ("Hash %u: {", j);
      for (size_t i = 0; i < 15; i++)
        printf ("%02x", hash[j][i]);
      printf ("%02x}\n", hash [j][15]);
    }
  
    delete [] data_buffers [0];
    delete [] data_buffers [1];
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
