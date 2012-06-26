#ifndef UTILITY_UTILS_HEADER
#define UTILITY_UTILS_HEADER

#include <sgi_stl/string>
#include <sgi_stl/vector>

namespace plarium
{

namespace utility
{

//Print to sgi_stl::string
sgi_stl::string format (const char* format,...);

//split string
sgi_stl::vector<sgi_stl::string> split (const char* str, const char* delimiters=" ", const char* brackets="");

//Get current time
size_t milliseconds ();

//get buffer size for compression
size_t zlib_compression_upper_size (size_t data_size);
//compress data, returns compressed size
size_t zlib_compress (const unsigned char* src, size_t src_size, unsigned char* dst, size_t dst_size);
//decompress data, throws exception if decompressed size not equal to dst_size
void zlib_decompress (const unsigned char* src, size_t src_size, unsigned char* dst, size_t dst_size);

}

}

#endif
