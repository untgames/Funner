#include "test.h"

const char* CHECK_FILE = "file.c";

int main ()
{
  printf ("Results of zip_mount_link_test:\n");
  
  try
  {
    FileSystem::AddSearchPath ("data");
    FileSystem::AddSearchPath ("crash_test");

    printf ("IsFileExist(%s): %s\n", CHECK_FILE, FileSystem::IsFileExist (CHECK_FILE) ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
