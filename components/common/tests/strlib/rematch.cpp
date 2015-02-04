#include <stdio.h>
#include <locale.h>
#include <common/strlib.h>

using namespace stl;
using namespace common;

struct Test
{
  const char* pattern;
  const char* text;
};

Test TESTS [] = {
 {
    "[12][[:digit:]][[:digit:]][[:digit:]]\\-[01][[:digit:]]\\-[0-3][[:digit:]]",
    "31643345"
 },
 {
    "[12][[:digit:]][[:digit:]][[:digit:]]\\-[01][[:digit:]]\\-[0-3][[:digit:]]",
    "2014-01-02"
 },
 {
    "[12][[:digit:]][[:digit:]][[:digit:]]\\-[01][[:digit:]]\\-[0-3][[:digit:]]",
    "hello world"
 },
 {
    "[12]",
    "1"
 },
};

int main ()
{
  printf ("Results of rematch_test:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    printf ("Rematch for test %lu is %d\n", i, rematch (TESTS [i].text, TESTS [i].pattern, "g"));
  }

  return 0;
}
