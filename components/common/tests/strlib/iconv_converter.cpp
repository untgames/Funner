#include <stdio.h>
#include <string.h>

#include <stl/string>

#include <common/strconv.h>
#include <common/file.h>
#include <common/streams.h>

using namespace common;

void dump (const void* buffer, size_t length, const char* encoding)
{
  const char* s = (const char*)buffer;

  for (size_t i=0; i<length; i++)
    printf ("%c", s [i]);
}

void test_converter (const char* file_name, const char* source_encoding)
{
  static const char* DST_ENCODING = "UTF-8";

  const size_t BUFFER_SIZE = 16384;

  printf ("test encoding: %s->%s:\n", source_encoding, DST_ENCODING);

  char buffer [2][BUFFER_SIZE];

  InputFile file (file_name);

  memset (buffer [0], 0, sizeof buffer [0]);
  memset (buffer [1], 0, sizeof buffer [1]);

  size_t source_length = file.Read (buffer [0], sizeof buffer [0]),
         dest_length   = sizeof buffer [1];

  const void* src_ptr = buffer [0];
  void*       dst_ptr = buffer [1];

  StringConverter (source_encoding, DST_ENCODING).Convert (src_ptr, source_length, dst_ptr, dest_length);

  dump (buffer [1], sizeof (buffer [1]) - dest_length, DST_ENCODING);
  
  printf ("\n");
}

int main ()
{
  printf ("Results of iconv_converter_test:\n");

  try
  {
    struct Entry
    {
      const char* file_name;
      const char* encoding;
    };
    
    Entry entries [] = {
      {"data/CP1255-snippet", "CP1255"},
      {"data/CP1258-snippet", "CP1258"},      
    };

    for (size_t i=0; i<sizeof (entries)/sizeof(*entries); i++)
      test_converter (entries [i].file_name, entries [i].encoding);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
