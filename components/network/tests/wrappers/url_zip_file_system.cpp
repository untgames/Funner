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
    
    stl::string source_file_path = common::format ("http://localhost:%u/test.zip", SERVER_PORT);

    common::FileSystem::AddSearchPath (source_file_path.c_str (), &print_search_log);
    
    common::FileSystem::CopyFile ("test.txt", "/io/stdout");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
