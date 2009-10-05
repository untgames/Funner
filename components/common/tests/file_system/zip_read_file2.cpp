#include "test.h"

const char* ZIP_NAME   = "data/crash_test.zip";
const char* FILE_NAME1 = "crash_test/zip.c";
const char* FILE_NAME2 = "crash_test/file.c";

int main ()
{
  printf ("Results of zip_read_file2_test:\n");
  
  try
  {
    FileSystem::AddSearchPath (ZIP_NAME);

    InputFile file1 (FILE_NAME1);

    {
      InputFile file2 (FILE_NAME2);
    }

    file1.Seek (1);

    char dummy_buffer;

    file1.Read (&dummy_buffer, sizeof (dummy_buffer));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  printf ("Test successful\n");

  return 0;
}
