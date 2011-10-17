#include "test.h"

const char* CHECK_FILE = "/mounted/test_zip.txt";

int main ()
{
  printf ("Results of zip_mount_link_test:\n");
  
  try
  {
    FileSystem::MountLink ("/mounted", "search");
    FileSystem::AddSearchPath ("data");

    printf ("IsFileExist(%s): %s\n", CHECK_FILE, FileSystem::IsFileExist (CHECK_FILE) ? "true" : "false");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
