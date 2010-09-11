#include "shared.h"

int main ()
{
  printf ("Results of url_file_system_test:\n");

  try
  {
//    common::LogFilter filter ("*", &print_log);
    
    common::FileSystem::CopyFile ("http://www.google.com", "/io/stdout");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
