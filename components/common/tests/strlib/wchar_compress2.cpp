#include <cstdio>

#include <xtl/exception.h>
#include <xtl/string.h>

#include <common/utf_converter.h>

using namespace common;

struct Test
{
  const wchar_t* source;
  size_t         source_length;
  char*          destination;
  size_t         max_destination_size;
};

const size_t DECOMPRESS_BUFFER_SIZE = 64;

int main ()
{
  printf ("Results of wchar_compress2:\n");

  char decompress_buffer[DECOMPRESS_BUFFER_SIZE];
  const wchar_t* source = L"Test";


  static Test test [] = {
      {0, 0, 0, 0},
      {0, 0, 0, DECOMPRESS_BUFFER_SIZE},
      {0, 0, decompress_buffer, 0},
      {0, 0, decompress_buffer, DECOMPRESS_BUFFER_SIZE},
      {0, xtl::xstrlen (source), 0, 0},
      {0, xtl::xstrlen (source), 0, DECOMPRESS_BUFFER_SIZE},
      {0, xtl::xstrlen (source), decompress_buffer, 0},
      {0, xtl::xstrlen (source), decompress_buffer, DECOMPRESS_BUFFER_SIZE},
      {source, 0, 0, 0},
      {source, 0, 0, DECOMPRESS_BUFFER_SIZE},
      {source, 0, decompress_buffer, 0},
      {source, 0, decompress_buffer, DECOMPRESS_BUFFER_SIZE},
      {source, xtl::xstrlen (source), 0, 0},
      {source, xtl::xstrlen (source), 0, DECOMPRESS_BUFFER_SIZE},
      {source, xtl::xstrlen (source), decompress_buffer, 0},
      {source, xtl::xstrlen (source), decompress_buffer, DECOMPRESS_BUFFER_SIZE}
  };

  static const size_t tests_count = sizeof (test) / sizeof (*test);

  for (size_t i = 0; i < tests_count; i++)
  {
    printf ("wchar_decompress (%S, %lu, %s, %lu) = ", test[i].source ? test[i].source : L"0",
             test[i].source_length, test[i].destination ? "buffer" : "0", test[i].max_destination_size);

    try
    {
      printf ("%lu\n", wchar_decompress (test[i].source, test[i].source_length, test[i].destination, test[i].max_destination_size));
    }
    catch (xtl::exception& exception)
    {
      printf ("'%s'\n", exception.what ());
    }
  }

  return 0;
}
