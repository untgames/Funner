#include <cstdio>

#include <stl/string>

#include <xtl/exception.h>
#include <xtl/string.h>

#include <common/utf_converter.h>

using namespace common;

const size_t DECOMPRESS_BUFFER_SIZE = 66;
const size_t COMPRESS_BUFFER_SIZE = 10;

int main()
{
  printf ("Results of utf16_compress_test:\n");  
 
  try
  {  
    const wchar_t* sources[] = {L"Test", L"Very big test", L""};
    char           decompress_buffer[DECOMPRESS_BUFFER_SIZE];
    wchar_t        compress_buffer[COMPRESS_BUFFER_SIZE];

    for (size_t i = 0; i < sizeof (sources) / sizeof (sources[0]); i++)
    {
      printf ("Processing string '%S':\n", sources[i]);

      printf ("  utf16_decompress (const wchar_t*, size_t, char*, size_t) result is %u; ", utf16_decompress (sources[i], xtl::xstrlen (sources[i]), decompress_buffer, DECOMPRESS_BUFFER_SIZE));
      printf ("decompressed string: '%s'\n", decompress_buffer);

      printf ("  utf16_decompress (const wchar_t*, char*, size_t) result is %u; ", utf16_decompress (sources[i], decompress_buffer, DECOMPRESS_BUFFER_SIZE));
      printf ("decompressed string: '%s'\n", decompress_buffer);

      printf ("  utf16_decompress (const wchar_t*, size_t) result is '%s'\n", utf16_decompress (sources[i], xtl::xstrlen (sources[i])).c_str ());
      printf ("  utf16_decompress (const wchar_t*) result is '%s'\n", utf16_decompress (sources[i]).c_str ());
      printf ("  utf16_decompress (const stl::wstring&) result is '%s'\n", utf16_decompress (stl::wstring (sources[i])).c_str ());

      printf ("  utf16_compress (const char*, size_t, wchar_t*, size_t) result is %u; ", utf16_compress (decompress_buffer, xtl::xstrlen (decompress_buffer),
                                                                                         compress_buffer, COMPRESS_BUFFER_SIZE));
      printf ("compressed string: %04x%04x%04x%04x or '%S'\n", compress_buffer[0], compress_buffer[1], compress_buffer[2], compress_buffer[3], compress_buffer);

      printf ("original string: %04x%04x%04x%04x\n", sources[i][0], sources[i][1], sources[i][2], sources[i][3], sources[i][4], sources[i][5]);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
