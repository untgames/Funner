#include "shared.h"

const size_t BUFFER_SIZE = 100000;

int main ()
{
  printf ("Results of url_connection1_test:\n");

  try
  {
    common::LogFilter filter ("*", &print_log);
    
//    UrlConnection connection ("http://www.google.com");
    UrlConnection connection ("http://www.untgames.com/files/SolarMacOSX.zip");
    
    printf ("content-type: '%s'\n", connection.ContentType ());
    printf ("content-encoding: '%s'\n", connection.ContentEncoding ());
    printf ("content-length: %u\n", connection.ContentLength ());
    
    xtl::uninitialized_storage<char> buffer (BUFFER_SIZE);
    
    size_t total_size = connection.ContentLength (), size, received_size = 0;
    
    size_t start_time = common::milliseconds ();
    
    while ((size = connection.Receive (buffer.data (), buffer.size ())) != 0)
    {
      received_size += size;
      
      printf ("\r                        \rreceived: %u/%u (%.1f%%, speed=%.2f MB/sec)",
        received_size, total_size, double (received_size) / total_size * 100.0f, received_size / double (common::milliseconds () - start_time) / 1000.0);
    }
    
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
