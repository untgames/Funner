#include "shared.h"

using namespace plarium::utility;

struct Test
{
  const char*   key;
  const char*   data;
  unsigned char result [32];
};

Test TESTS [] = {
 { "", "",  { 0xb6, 0x13, 0x67, 0x9a, 0x08, 0x14, 0xd9, 0xec, 0x77, 0x2f, 0x95, 0xd7, 0x78, 0xc3, 0x5f, 0xc5, 0xff, 0x16, 0x97, 0xc4, 0x93, 0x71, 0x56, 0x53, 0xc6, 0xc7, 0x12, 0x14, 0x42, 0x92, 0xc5, 0xad } },
 { "key", "The quick brown fox jumps over the lazy dog", { 0xf7, 0xbc, 0x83, 0xf4, 0x30, 0x53, 0x84, 0x24, 0xb1, 0x32, 0x98, 0xe6, 0xaa, 0x6f, 0xb1, 0x43, 0xef, 0x4d, 0x59, 0xa1, 0x49, 0x46, 0x17, 0x59, 0x97, 0x47, 0x9d, 0xbc, 0x2d, 0x1a, 0x3c, 0xd8 } }
};

int main ()
{
  printf ("Results of hmac_sha256_test:\n");

  unsigned char hash [32];

  printf ("One-time hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    hmac_sha256 (TESTS [i].key, strlen (TESTS [i].key), TESTS [i].data, strlen (TESTS [i].data), hash);

    printf ("HMAC for key '%s' and data '%s' correct - %c\n", TESTS [i].key, TESTS [i].data, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  printf ("Context usage hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    HmacSha256Context context (TESTS [i].key, strlen (TESTS [i].key));

    context.Update (TESTS [i].data, strlen (TESTS [i].data));
    context.Finish (hash);

    printf ("HMAC for key '%s' and data '%s' correct - %c\n", TESTS [i].key, TESTS [i].data, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  return 0;
}
