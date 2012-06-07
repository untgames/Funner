#include "shared.h"

using namespace plarium::utility;

const unsigned char source [] = { 0x0d, 0xd7, 0xb8, 0x1d, 0x02, 0x2a, 0xe6, 0xbd, 0x1b, 0x5a, 0xaa, 0xca, 0x5d, 0xb8, 0x88, 0xcb, 0x5b, 0xb0, 0x64, 0x96, 0xf6, 0x03, 0xf8, 0xf9 };
const unsigned char result [] = { 0x78, 0x9c, 0xe3, 0xbd, 0xbe, 0x43, 0x96, 0x49, 0xeb, 0xd9, 0x5e, 0xe9, 0xa8, 0x55, 0xa7, 0x62, 0x77, 0x74, 0x9c, 0x8e, 0xde, 0x90, 0x32, 0xed, 0x1b, 0xf3, 0x8f, 0x9f, 0x00, 0x8d, 0xc4, 0x0c, 0xc9 };

int main ()
{
  printf ("Results of zlib_test:\n");

  size_t buffer_size = zlib_compression_upper_size (sizeof (source));

  printf ("Buffer size for compression = %u\n", buffer_size);

  unsigned char* compress_buffer = new unsigned char [buffer_size];

  size_t compressed_size = zlib_compress (source, sizeof (source), compress_buffer, buffer_size);

  printf ("Compressed size = %u\n", compressed_size);

  if (compressed_size == sizeof (result))
    printf ("Compressed size correct, compressed data correct - '%c'\n", memcmp (compress_buffer, result, sizeof (result)) ? 'n' : 'y');
  else
    printf ("Invalid compressed size\n");

  unsigned char decompress_buffer [sizeof (source)];

  zlib_decompress (compress_buffer, compressed_size, decompress_buffer, sizeof (decompress_buffer));

  printf ("Decompression correct = '%c'\n", memcmp (decompress_buffer, source, sizeof (source)) ? 'n' : 'y');

  delete [] compress_buffer;

  return 0;
}
