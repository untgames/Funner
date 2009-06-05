#include <xtl/common_exceptions.h>

#include <common/strlib.h>

namespace common
{

/*
    Компрессия / декомпрессия последовательности байт в символы
*/

void decompress_buffer (size_t buffer_size, const void* src_buffer, char* dst_buffer)
{
  if (buffer_size)
  {
    if (!src_buffer)
      throw xtl::make_null_argument_exception ("common::decompress_buffer", "src_buffer");

    if (!dst_buffer)
      throw xtl::make_null_argument_exception ("common::decompress_buffer", "dst_buffer");
  }

  const char* src = (const char*)src_buffer;
  char*       dst = dst_buffer;

  for (size_t i=0; i<buffer_size; i++, src++, dst += 2)
  {    
    xtl::xsnprintf (dst, 3, "%02x", *src);
  }
}

void compress_buffer (size_t buffer_size, const char* src_buffer, void* dst_buffer)
{
  if (buffer_size)
  {
    if (!src_buffer)
      throw xtl::make_null_argument_exception ("common::compress_buffer", "src_buffer");

    if (!dst_buffer)
      throw xtl::make_null_argument_exception ("common::compress_buffer", "dst_buffer");
  }
  
  buffer_size /= 2;

  const char* src = src_buffer;
  char*       dst = (char*)dst_buffer;

  for (size_t i=0; i < buffer_size; i++, src += 2, dst++)
  {
    char  conversion_buffer [] = {src [0], src [1], '\0'};
    char* end = 0;        

    *dst = (char)(size_t)strtol (conversion_buffer, &end, 16);
  }
}

}
