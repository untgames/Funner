#include <cstdio>
#include <climits>
#include <cstring>
#include <exception>

#include <common/crypto.h>

using namespace common;

struct Test
{
  unsigned short      key_size;
  size_t              data_size;
  const unsigned char key [32];
  const unsigned char iv [16];
  const unsigned char source [24];
  const unsigned char result [24];
};

Test TESTS [] = {
 { 16,
   24,
   { 0xa2, 0x62, 0xe1, 0xab, 0xd5, 0xbc, 0x42, 0xad, 0x30, 0x04, 0x42, 0xd0, 0x8b, 0xc2, 0x53, 0x48 },
   { 0x1c, 0x92, 0x88, 0x31, 0x0c, 0x6a, 0xc0, 0xae, 0x7d, 0x59, 0x75, 0xa4, 0x69, 0x47, 0xaa, 0xf8 },
   { 0xb6, 0xce, 0xd4, 0x1d, 0xed, 0xa1, 0x87, 0xa7, 0xea, 0xe5, 0x5d, 0x49, 0x7d, 0x7d, 0x73, 0x24, 0xb5, 0xb7, 0x23, 0x48, 0xa2, 0x60, 0xda, 0x00 },
   { 0x47, 0x21, 0x0f, 0x4f, 0x79, 0xce, 0x1e, 0x2e, 0xda, 0xaa, 0xad, 0x64, 0x3a, 0xc1, 0x05, 0x8d, 0x90, 0x0b, 0x76, 0x69, 0xd6, 0x03, 0x38, 0x74 }
 },
 { 24,
   24,
   { 0x0d, 0xd7, 0xb8, 0x1d, 0x02, 0x2a, 0xe6, 0xbd, 0x1b, 0x5a, 0xaa, 0xca, 0x5d, 0xb8, 0x88, 0xcb, 0x5b, 0xb0, 0x64, 0x96, 0xf6, 0x03, 0xf8, 0xf9 },
   { 0xa6, 0x14, 0x14, 0xf2, 0x2e, 0x45, 0x81, 0x1d, 0x1e, 0x42, 0xc7, 0x05, 0x50, 0xda, 0x12, 0x1a },
   { 0x40, 0xd8, 0x83, 0x42, 0x74, 0x49, 0xc5, 0x59, 0x35, 0x39, 0x47, 0x06, 0xf2, 0x71, 0x91, 0x0b, 0x25, 0x10, 0x70, 0x7b, 0xef, 0xa2, 0xbc, 0x38 },
   { 0x86, 0x1a, 0xdd, 0xa6, 0xf5, 0xd6, 0x68, 0x9f, 0xd2, 0x2f, 0x18, 0xbe, 0xf8, 0x2f, 0x40, 0x2d, 0xa4, 0x06, 0xaf, 0x88, 0x0f, 0xb4, 0x9c, 0x74 }
 },
 { 32,
   24,
   { 0x71, 0x55, 0x90, 0x9c, 0x80, 0x95, 0xc9, 0x76, 0x26, 0x86, 0x85, 0xe1, 0xed, 0x98, 0x89, 0xc4, 0x2f, 0x15, 0x05, 0x40, 0xf7, 0x04, 0x66, 0x91, 0xb2, 0x71, 0xbb, 0x4b, 0x68, 0x21, 0xd2, 0xf4 },
   { 0x4e, 0xe3, 0xea, 0x04, 0xaa, 0x4e, 0x26, 0x7b, 0xb8, 0x9e, 0xb2, 0x6f, 0xd1, 0xe5, 0x9b, 0x6d },
   { 0xb2, 0xe2, 0x1f, 0xdf, 0x7a, 0xa6, 0x2a, 0x8b, 0xdb, 0x54, 0x37, 0x43, 0x75, 0xc5, 0x8a, 0xb4, 0x16, 0x85, 0x58, 0x7d, 0xbf, 0xca, 0xa4, 0x89 },
   { 0x55, 0xe5, 0x58, 0xf1, 0x2a, 0x00, 0x7c, 0xff, 0x03, 0xfb, 0xda, 0x2e, 0x20, 0x98, 0x08, 0x03, 0xef, 0x2a, 0xfc, 0x53, 0x24, 0xa3, 0x63, 0x93 }
 },
 { 16,
   13,
   { 0x2E, 0xDC, 0xA4, 0xAA, 0xB6, 0xB5, 0x1A, 0xB3, 0xEA, 0xBE, 0xC9, 0x7B, 0x2B, 0xA7, 0x4D, 0x4E },
   { 0xEE, 0xA3, 0x7D, 0xEE, 0x36, 0x03, 0x2E, 0xB1, 0xEF, 0x0F, 0x1A, 0xE0, 0xE5, 0x14, 0xA7, 0x5E },
   { 'O', 'F', 'B', ' ', 'M', 'o', 'd', 'e', ' ', 'T', 'e', 's', 't' },
   { 0xC1, 0xEC, 0xE8, 0xAB, 0xAB, 0xF8, 0xDD, 0x92, 0xE8, 0x6B, 0xD5, 0x67, 0x42 }
 }
};

int main ()
{
  printf ("Results of aes_ofb_test:\n");

  unsigned char buffer [24], decrypt_buffer [24];

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    CryptoContext context ("aes.ofb.encrypt", TESTS [i].key, TESTS [i].key_size * 8, (const char*)TESTS [i].iv);

    for (size_t j = 0; j < TESTS [i].data_size; j++)
      context.Update (1, TESTS [i].source + j, buffer + j);

    printf ("Crypto %d correct - %c\n", i, memcmp (buffer, TESTS [i].result, TESTS [i].data_size) ? 'n' : 'y');

    CryptoContext decrypt_context ("aes.ofb.decrypt", TESTS [i].key, TESTS [i].key_size * 8, (const char*)TESTS [i].iv);

    for (size_t j = 0; j < TESTS [i].data_size; j++)
      decrypt_context.Update (1, buffer + j, decrypt_buffer + j);

    printf ("Decrypt correct - %c\n", memcmp (TESTS [i].source, decrypt_buffer, TESTS [i].data_size) ? 'n' : 'y');
  }

  return 0;
}
