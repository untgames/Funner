#include <common/strlib.h>
#include <stdio.h>

int main ()
{
  printf ("Results of regexp2_test:\n");

  try
  {
    const char* string  = "X=Y A=E CC=DD";
    const char* pattern = "([[:alpha:]]*)=([[:alpha:]])";

    common::StringArray tokens = common::parse (string, pattern, "g");

    printf ("Found %lu matches:\n", tokens.Size ());

    for (size_t i=0;i<tokens.Size ();i++)
      printf ("    '%s'\n",tokens [i]);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
