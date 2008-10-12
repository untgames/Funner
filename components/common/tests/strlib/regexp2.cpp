#include <common/strlib.h>
#include <stdio.h>

int main ()
{
  printf ("Results of regexp2_test:\n");

  try
  {
    typedef stl::vector<stl::string> TokenList;

    const char* string  = "X=Y A=E CC=DD";
    const char* pattern = "([[:alpha:]]*)=([[:alpha:]])";

    TokenList tokens = common::parse (string, pattern, "g");

    printf ("Found %lu matches:\n", tokens.size ());

    for (size_t i=0;i<tokens.size ();i++)
      printf ("    '%s'\n",tokens [i].c_str ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
