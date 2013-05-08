#include "test.h"
#include <common/log.h>

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
}

int main ()
{
  printf ("Results of search_path_zip_test:\n");

  common::LogFilter filter ("common.file*", &log_print);

  try
  {
    FileSystem::AddSearchPath ("data/test.zip");
    FileSystem::AddSearchPath ("search");

    printf ("check file exist: %s\n", FileSystem::IsFileExist ("test_zip.txt") ? "true" : "false");

    FileSystem::RemoveSearchPath ("search");

    printf ("check file exist: %s\n", FileSystem::IsFileExist ("test_zip.txt") ? "true" : "false");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
