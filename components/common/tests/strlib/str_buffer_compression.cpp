#include <cstring>
#include <common/strlib.h>

using namespace common;

int main ()
{
  printf ("Results of str_buffer_compression_test:\n");
  
  char src [] = "abcd";
  char dst1 [sizeof (src) * 2];
  char dst2 [sizeof (src)];  
  
  printf ("source:           '%s'\n", src);
  
  decompress_buffer (strlen (src), src, dst1);
  
  printf ("after decompress: '%s'\n", dst1);
  
  dst2 [strlen (src)] = '\0';  
  
  compress_buffer (strlen (dst1), dst1, dst2);  
  
  printf ("after compress:   '%s'\n", dst2);  

  return 0;
}
