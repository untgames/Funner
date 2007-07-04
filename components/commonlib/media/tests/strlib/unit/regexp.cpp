#include <stdio.h>
#include <locale.h>
#include <common/strlib.h>

using namespace stl;
using namespace common;

int main ()
{
  printf ("Results of regexp_test:\n");  

  const char* pattern = "(hello) ([[:lower:]]+)";
  const char* str     = "hello world";  

  vector<string> tokens = parse (str,pattern);  

  printf ("parse '%s' (pattern='%s', locale='%s') on %u tokens:\n",
          str,pattern,setlocale (LC_ALL,NULL),tokens.size ());

  for (size_t i=0;i<tokens.size ();i++)
    printf ("    '%s'\n",tokens [i].c_str ());    

  return 0;
}
