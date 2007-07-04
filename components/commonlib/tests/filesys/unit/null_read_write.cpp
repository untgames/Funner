#include "test.h"

const char* SRC_FILE_NAME = "media/tests/filesys/test.txt";
const char* DST_FILE_NAME = "media/tests/filesys/test_copy.txt";

int main ()
{
  printf ("Results of null_read_write_test:\n");

  try
  {
    InputFile  file1 (SRC_FILE_NAME);
    OutputFile file2 (DST_FILE_NAME);
    
    printf ("File::Read(0,0): %d\n", file1.Read (0,0));
    printf ("File::Write(0,0): %d\n", file2.Write (0,0));

    file2.Close ();

    FileSystem::Remove (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
