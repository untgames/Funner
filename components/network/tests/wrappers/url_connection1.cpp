#include <cstdio>
#include <exception>

#include <network/url_connection.h>

using namespace network;

int main ()
{
  printf ("Results of url_connection1_test:\n");

  try
  {
    UrlConnection connection ("http://www.google.com");
    
    while (connection.ReceiveAvailable ())
    {
      char buffer [16];
      
      size_t size = connection.Receive (buffer, sizeof (buffer) - 1);
      
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
