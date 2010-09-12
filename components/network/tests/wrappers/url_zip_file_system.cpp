#include "shared.h"

void print_search_log (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of url_zip_file_system_test:\n");

  try
  {
//    common::LogFilter filter ("network.*", &print_log);
    
    TestHttpServer http_server;
    
    common::FileSystem::AddSearchPath ("http://localhost:8080/test.zip", &print_search_log);
    
    common::FileSystem::CopyFile ("test.txt", "/io/stdout");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
