#include "shared.h"

using namespace plarium::utility;

const char* TESTS [] = {
  "this is test 1",
  " this is test 2 ",
  " this 'is test 3' ''"
};

int main ()
{
  printf ("Results of split_test:\n");

  for (size_t i = 0, count = sizeof (TESTS) / sizeof (*TESTS); i < count; i++)
  {
    std::vector<std::string> words = split (TESTS [i], " ", "'");

    printf ("Words for string '%s':\n", TESTS [i]);

    for (size_t j = 0, count = words.size (); j < count; j++)
      printf ("  '%s'\n", words [j].c_str ());
  }

  return 0;
}
