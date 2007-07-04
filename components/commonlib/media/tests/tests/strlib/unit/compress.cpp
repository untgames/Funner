#include <common/strlib.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of compress_test:\n");
  
  const char* s1 = "This is hello\\b\\b\\b\\b\\bworld string with '\\132'\\n";
  
  printf ("source string:\n");  
  printf ("%s\n",s1);
  printf ("compressed string:\n");
  printf ("%s",compress (s1).c_str ());  
  printf ("decompressed string:\n");
  printf ("%s\n",decompress (compress (s1).c_str ()).c_str ());

  return 0;
}
