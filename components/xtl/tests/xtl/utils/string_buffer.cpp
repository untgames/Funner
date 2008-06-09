#include <stdio.h>
#include <xtl/string_buffer.h>

using namespace xtl;

int main ()
{
  printf ("Results of string_buffer_test:\n");
  
  string_buffer buf1;
  
  buf1.append_format ("Hello world %d", 1);
  
  printf ("buf1.size ()=%u strlen (buf1.data ())=%u\n", buf1.size (), strlen (buf1.data ()));
  
  buf1 += " !!!";
  
  string_buffer buf2;
  
  printf ("buf1 = '%s' buf2='%s'\n", buf1.data (), buf2.data ());
  printf ("buf1.empty (): %s\n", buf1.empty () ? "true" : "false");
  printf ("buf2.empty (): %s\n", buf2.empty () ? "true" : "false");

  printf ("buf1.size ()=%u strlen (buf1.data ())=%u\n", buf1.size (), strlen (buf1.data ()));
  printf ("buf2.size ()=%u strlen (buf2.data ())=%u\n", buf2.size (), strlen (buf2.data ()));

  buf2 = buf1 + " (after append string)";
  
  printf ("buf1 = '%s' buf2='%s'\n", buf1.data (), buf2.data ());
  printf ("buf1.empty (): %s\n", buf1.empty () ? "true" : "false");
  printf ("buf2.empty (): %s\n", buf2.empty () ? "true" : "false");

  printf ("buf1.size ()=%u strlen (buf1.data ())=%u\n", buf1.size (), strlen (buf1.data ()));
  printf ("buf2.size ()=%u strlen (buf2.data ())=%u\n", buf2.size (), strlen (buf2.data ()));

  return 0;
}
