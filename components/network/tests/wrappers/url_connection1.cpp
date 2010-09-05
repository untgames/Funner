#include "shared.h"

int main ()
{
  printf ("Results of url_connection1_test:\n");

  try
  {
    common::LogFilter filter ("*", &print_log);
    
    UrlConnection connection ("http://www.google.com");
    
    printf ("content-type: '%s'\n", connection.ContentType ());
    printf ("content-encoding: '%s'\n", connection.ContentEncoding ());
    printf ("content-length: %u\n", connection.ContentLength ());
    
    char buffer [16];
    
    size_t size;
    
    while ((size = connection.Receive (buffer, sizeof (buffer) - 1)) != 0)
    {
      buffer [size] = 0;
      
      printf ("%s", buffer);
    }
    
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
