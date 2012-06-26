#include "shared.h"

using namespace plarium::utility;

namespace
{

const int COMPRESSION_LEVEL = Z_DEFAULT_COMPRESSION;

const char* get_error_name (int error)
{
  switch (error)
  {
    case Z_STREAM_END:    return "Stream end";
    case Z_NEED_DICT:     return "Need dict";
    case Z_STREAM_ERROR:  return "Stream error";
    case Z_DATA_ERROR:    return "Data error";
    case Z_MEM_ERROR:     return "Memory error";
    case Z_BUF_ERROR:     return "Buffer error";
    case Z_VERSION_ERROR: return "Version error";
    default:              return "Unknown";
  }
}

void check_errors (int error, const char* source)
{
  if (error == Z_OK)
    return;

  throw sgi_stl::runtime_error (format ("Error at '%s' - '%s'", source, get_error_name (error)));
}

}

namespace plarium
{

namespace utility
{

//Zlib compression
size_t zlib_compression_upper_size (size_t data_size)
{
  z_stream strm;

  memset (&strm, 0, sizeof (strm));

  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;

  int result = deflateInit (&strm, COMPRESSION_LEVEL);

  check_errors (result, "::deflate_init");

  size_t return_value = deflateBound (&strm, data_size);

  deflateEnd (&strm);

  return return_value;
}

size_t zlib_compress (const unsigned char* src, size_t src_size, unsigned char* dst, size_t dst_size)
{
  z_stream strm;

  memset (&strm, 0, sizeof (strm));

  strm.zalloc    = Z_NULL;
  strm.zfree     = Z_NULL;
  strm.opaque    = Z_NULL;
  strm.next_in   = const_cast<Bytef*> (src);
  strm.avail_in  = src_size;
  strm.next_out  = dst;
  strm.avail_out = dst_size;

  int result = deflateInit (&strm, COMPRESSION_LEVEL);

  check_errors (result, "::deflate_init");

  result = deflate (&strm, Z_FINISH);

  int return_value = strm.total_out;
  int end_result   = deflateEnd (&strm);

  if (result == Z_OK)
    throw sgi_stl::invalid_argument ("Insufficient dst_size");

  if (result != Z_STREAM_END)
    check_errors (result, "::deflate");

  check_errors (end_result, "::deflateEnd");

  return return_value;
}

void zlib_decompress (const unsigned char* src, size_t src_size, unsigned char* dst, size_t dst_size)
{
  z_stream strm;

  memset (&strm, 0, sizeof (strm));

  strm.zalloc    = Z_NULL;
  strm.zfree     = Z_NULL;
  strm.opaque    = Z_NULL;
  strm.next_in   = const_cast<Bytef*> (src);
  strm.avail_in  = src_size;
  strm.next_out  = dst;
  strm.avail_out = dst_size;

  int result = inflateInit (&strm);

  check_errors (result, "::inflate_init");

  result = inflate (&strm, Z_FINISH);

  int dst_available = strm.avail_out;
  int end_result    = inflateEnd (&strm);

  if (result == Z_OK)
    throw sgi_stl::invalid_argument ("Insufficient dst_size");

  if (result != Z_STREAM_END)
    check_errors (result, "::inflate");

  check_errors (end_result, "::inflateEnd");

  if (dst_available)
    throw sgi_stl::invalid_argument ("dst_size too large");
}

}

}
