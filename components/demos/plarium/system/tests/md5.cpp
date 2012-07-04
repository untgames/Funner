#include "shared.h"

using namespace plarium::utility;

struct Test
{
  const char*   source;
  unsigned char result [16];
};

Test TESTS [] = {
 { "", { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } },
 { "The quick brown fox jumps over the lazy dog", { 0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82, 0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6 } }
};

int main ()
{
  printf ("Results of md5_test:\n");

  unsigned char hash [16];

  printf ("One-time hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    md5 (TESTS [i].source, strlen (TESTS [i].source), hash);

    printf ("Hash for '%s' correct - %c\n", TESTS [i].source, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  printf ("Context usage hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    Md5Context context;

    for (size_t j = 0, count = strlen (TESTS [i].source); j < count; j++)
      context.Update (TESTS [i].source + j, 1);

    context.Finish (hash);

    printf ("Hash for '%s' correct - %c\n", TESTS [i].source, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  return 0;
}
