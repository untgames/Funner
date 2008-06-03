#include <stdio.h>
#include <string.h>

#include <stl/string>

#include <common/strconv.h>
#include <common/file.h>
#include <common/streams.h>

using namespace common;

void dump (const void* buffer, size_t length, const char* encoding)
{
  if (!strcmp (encoding, "ASCII7"))
  {
    const char* s = (const char*)buffer;
  
    for (size_t i=0; i<length; i++)
      printf ("%c", s [i]);    
  }
  else if (!strcmp (encoding, "UTF8"))
  {
    const char* s = (const char*)buffer;

    for (size_t i=0; i<length; i++)
      printf ("%c", s [i]);
  }
  else if (!strcmp (encoding, "UTF16LE"))
  {
    const wchar_t* s = (const wchar_t*)buffer;

    for (size_t i=0; i<length/2; i++)
      printf ("%C", s [i]);
  }
  else if (!strcmp (encoding, "UTF16BE"))
  {
    const wchar_t* s = (const wchar_t*)buffer;

    for (size_t i=0; i<length/2; i++)
      printf ("%C", (s [i]>>8)|((s [i]<<8)&0xFF00));    
  }
}

void test_converter (const char* file_name, const char* source_encoding)
{
  const size_t BUFFER_SIZE = 16384;
  
  static const char* encoding [4] = {"ASCII7", "UTF8", "UTF16LE", "UTF16BE"};

  for (size_t i=0; i<4; i++)
  {
    printf ("test encoding chain: %s->UTF32LE->%s: '", source_encoding, encoding [i]);
  
    char buffer [2][BUFFER_SIZE];

    InputFile file (file_name);

    memset (buffer [0], 0, sizeof buffer [0]);
    memset (buffer [1], 0, sizeof buffer [1]);
    
    size_t source_length = file.Read (buffer [0], sizeof buffer [0]),
           dest_length   = sizeof buffer [1];
 
    const void* src_ptr = buffer [0];
    void*       dst_ptr = buffer [1];
    
    StringConverter (source_encoding, encoding [i]).Convert (src_ptr, source_length, dst_ptr, dest_length);

    dump (buffer [1], sizeof (buffer [1]) - dest_length, encoding [i]);

    printf ("'\n");
  }
}

int main ()
{
  printf ("Results of string_converter_test:\n");  

  try
  {  
    const char* source_file_name [4] = {"data\\ansi.txt", "data\\utf8.txt", "data\\utf16le.txt","data\\utf16be.txt"};
    const char* encoding [4] = {"ASCII7", "UTF8", "UTF16LE", "UTF16BE"};

    for (size_t i=0; i<4; i++)
      test_converter (source_file_name [i], encoding [i]);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
