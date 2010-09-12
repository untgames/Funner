#include "shared.h"

int main ()
{
  printf ("Results of url_file_system_test:\n");

  try
  {
//    common::LogFilter filter ("network.*", &print_log);
    
    TestHttpServer http_server;
    
    common::FileSystem::CopyFile ("http://localhost:8080/test.txt", "/io/stdout");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
