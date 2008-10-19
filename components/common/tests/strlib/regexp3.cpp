#include <stdio.h>
#include <common/strlib.h>

using namespace stl;
using namespace common;

int main ()
{
  printf ("Results of regexp3_test:\n");

  const char* pattern = "dynamic *\\( *([[:digit:]]*) *, *([[:digit:]]*) *, *'(.*)' *\\).*";
  const char* str     = "dynamic (256 , 512,'test_query')???";

  StringArray tokens = parse (str,pattern);

  printf ("parse '%s' (pattern='%s') on %lu tokens:\n", str,pattern,tokens.Size ());

  for (size_t i=0;i<tokens.Size ();i++)
    printf ("    '%s'\n",tokens [i]);

  return 0;
}
