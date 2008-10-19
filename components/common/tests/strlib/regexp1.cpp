#include <stdio.h>
#include <locale.h>
#include <common/strlib.h>

using namespace stl;
using namespace common;

int main ()
{
  printf ("Results of regexp1_test:\n");

  const char* pattern = "(hello) ([[:lower:]]+)";
  const char* str     = "hello world";

  StringArray tokens = parse (str,pattern);

  printf ("parse '%s' (pattern='%s', locale='%s') on %lu tokens:\n",
          str,pattern,setlocale (LC_ALL,NULL),tokens.Size ());

  for (size_t i=0;i<tokens.Size ();i++)
    printf ("    '%s'\n",tokens [i]);

  return 0;
}
