#include <stdio.h>
#include <string.h>

#include <stl/string>

#include <common/utf_converter.h>
#include <common/file.h>
#include <common/streams.h>

using namespace common;

void dump (const void* buffer, size_t length, Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_ASCII7:
    {
      const char* s = (const char*)buffer;

      for (size_t i=0; i<length; i++)
        printf ("%c", s [i]);

      break;
    }
    case Encoding_UTF8:
    {
      const char* s = (const char*)buffer;

      for (size_t i=0; i<length; i++)
        printf ("%c", s [i]);

      break;
    }
    case Encoding_UTF16LE:
    {
      const unsigned short* s = (const unsigned short*)buffer;

      for (size_t i=0; i<length/2; i++)
        printf ("%C", s [i]);

      break;
    }
    case Encoding_UTF16BE:
    {
      const unsigned short* s = (const unsigned short*)buffer;

      for (size_t i = 0; i < length / 2; i++)
        printf ("%C", ((s [i] >> 8) & 0xFF) | ((s [i] << 8) & 0xFF00));

      break;
    }
    default:
      break;
  }
}

const char* get_encoding_name (Encoding encoding)
{
  switch (encoding)
  {
    case Encoding_ASCII7:  return "ASCII7";
    case Encoding_UTF8:    return "UTF8";
    case Encoding_UTF16LE: return "UTF16LE";
    case Encoding_UTF16BE: return "UTF16BE";
    default:               return "Unknown";
  }
}

void test_utf_converter (const char* file_name, Encoding source_encoding)
{
  const size_t BUFFER_SIZE = 16384;

  Encoding encoding [4] = {Encoding_ASCII7, Encoding_UTF8, Encoding_UTF16LE, Encoding_UTF16BE};

  for (size_t i=0; i<4; i++)
  {
    printf ("test encoding chain: %s->UTF32LE->%s: '", get_encoding_name (source_encoding), get_encoding_name (encoding [i]));

    char buffer [2][BUFFER_SIZE];

    InputFile file (file_name);

    memset (buffer [0], 0, sizeof buffer [0]);
    memset (buffer [1], 0, sizeof buffer [1]);

    size_t source_length = file.Read (buffer [0], sizeof buffer [0]),
           dest_length   = sizeof buffer [1];

    const void* src_ptr = buffer [0];
    void*       dst_ptr = buffer [1];

    convert_encoding (source_encoding, src_ptr, source_length, encoding [i], dst_ptr, dest_length);

    dump (buffer [1], sizeof (buffer [1]) - dest_length, encoding [i]);

    printf ("'\n");
  }
}

int main()
{
  printf ("Results of str_utf_converter_test:\n");

  try
  {
    const char* source_file_name [4] = {"data/ansi.txt", "data/utf8.txt", "data/utf16le.txt","data/utf16be.txt"};
    Encoding    source_encoding [4]  = {Encoding_ASCII7, Encoding_UTF8, Encoding_UTF16LE, Encoding_UTF16BE};

    for (size_t i=0; i<4; i++)
      test_utf_converter (source_file_name [i], source_encoding [i]);

    const wchar_t* wc = L"Hello World!!!";
    const char*    c = "Hello World!!!";

    printf ("wchar->char %s\n", tostring (wc).c_str());
    printf ("wchar->char %S\n", towstring (c).c_str ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
