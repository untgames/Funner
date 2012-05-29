#include "shared.h"

using namespace plarium::utility;

struct Test
{
  const char*   source;
  unsigned char result [32];
};

Test TESTS [] = {
 { "", { 0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55 } },
 { "The quick brown fox jumps over the lazy dog", { 0xd7, 0xa8, 0xfb, 0xb3, 0x07, 0xd7, 0x80, 0x94, 0x69, 0xca, 0x9a, 0xbc, 0xb0, 0x08, 0x2e, 0x4f, 0x8d, 0x56, 0x51, 0xe4, 0x6d, 0x3c, 0xdb, 0x76, 0x2d, 0x02, 0xd0, 0xbf, 0x37, 0xc9, 0xe5, 0x92 } }
};

int main ()
{
  printf ("Results of sha256_test:\n");

  unsigned char hash [32];

  printf ("One-time hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    sha256 (TESTS [i].source, strlen (TESTS [i].source), hash);

    printf ("Hash for '%s' correct - %c\n", TESTS [i].source, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  printf ("Context usage hash results:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    Sha256Context context;

    context.Update (TESTS [i].source, strlen (TESTS [i].source));
    context.Finish (hash);

    printf ("Hash for '%s' correct - %c\n", TESTS [i].source, memcmp (hash, TESTS [i].result, sizeof (hash)) ? 'n' : 'y');
  }

  return 0;
}
