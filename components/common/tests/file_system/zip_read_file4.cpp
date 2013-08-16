#include "test.h"

const char* MOUNT_PREFIX  = "/my/zip";
const char* ZIP_FILE_NAME = "data/test.my_zip";
const char* FILE_NAME     = "search/test_zip.txt";

int main ()
{
  printf ("Results of zip_read_file4_test:\n");

  try
  {
    FileSystem::Mount (MOUNT_PREFIX, ZIP_FILE_NAME, "zip");
    FileSystem::AddSearchPath (MOUNT_PREFIX);
    
    InputFile file (FILE_NAME);

    printf ("File name: '%s'\n",FILE_NAME);
    printf ("File size: %lu\n",file.Size ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
