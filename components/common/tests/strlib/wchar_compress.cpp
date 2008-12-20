#include <cstdio>

#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/string.h>

#include <common/utf_converter.h>

using namespace common;

const size_t DECOMPRESS_BUFFER_SIZE = 50;
const size_t COMPRESS_BUFFER_SIZE = 10;

int main()
{
  printf ("Results of wchar_compress_test:\n");

  try
  {
    stl::wstring  sources [] = {L"xx", L"Test", L"Very big test", L""};
    char          decompress_buffer[DECOMPRESS_BUFFER_SIZE];
    wchar_t       compress_buffer[COMPRESS_BUFFER_SIZE];

    if (sizeof (wchar_t) == 2)  //запись символа "скрипичный ключ"
    {
      sources [0] [0] = 0xD834;
      sources [0] [1] = 0xDD1E;
    }
    else if (sizeof (wchar_t) == 4)
    {
      sources [0] [0] = (wchar_t)0x1D11E;
      sources [0] [1] = 0;
    }
    else
      throw xtl::format_operation_exception ("main", "Unsupported wchar_t size %u", sizeof (wchar_t));

    for (size_t i = 0; i < sizeof (sources) / sizeof (sources[0]); i++)
    {
      memset (decompress_buffer, 0, sizeof (decompress_buffer));
      memset (compress_buffer, 0, sizeof (compress_buffer));

      printf ("Processing string '%u':\n", i);

      printf ("  wchar_decompress (const wchar_t*, size_t, char*, size_t) next char %04X; ", sources [i] [wchar_decompress (sources[i].c_str (), sources[i].length (), decompress_buffer, DECOMPRESS_BUFFER_SIZE)]);
      printf ("decompressed string: '%s'\n", decompress_buffer);

      printf ("  wchar_decompress (const wchar_t*, char*, size_t) next char %04X; ", sources [i] [wchar_decompress (sources[i].c_str (), decompress_buffer, DECOMPRESS_BUFFER_SIZE)]);
      printf ("decompressed string: '%s'\n", decompress_buffer);

      printf ("  wchar_decompress (const wchar_t*, size_t) result is '%s'\n", wchar_decompress (sources[i].c_str (), sources[i].length ()).c_str ());
      printf ("  wchar_decompress (const wchar_t*) result is '%s'\n", wchar_decompress (sources[i].c_str ()).c_str ());
      printf ("  wchar_decompress (const stl::wstring&) result is '%s'\n", wchar_decompress (sources[i]).c_str ());

      printf ("  wchar_compress (const char*, size_t, wchar_t*, size_t) result is %lu\n", wchar_compress (decompress_buffer, xtl::xstrlen (decompress_buffer),
                                                                                         compress_buffer, COMPRESS_BUFFER_SIZE));
 //     printf ("compressed string: %08X%08X%04X%04X or '%S'\n", compress_buffer[0], compress_buffer[1], compress_buffer[2], compress_buffer[3], compress_buffer);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
