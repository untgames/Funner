#include "shared.h"

int main ()
{
  printf ("Results of url_file_system_test:\n");

  try
  {
//    common::LogFilter filter ("network.*", &print_log);
    
    TestHttpServer http_server;
    
    stl::string source_file_path = common::format ("http://localhost:%u/test.txt", SERVER_PORT);

    common::FileSystem::CopyFile (source_file_path.c_str (), "/io/stdout");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
