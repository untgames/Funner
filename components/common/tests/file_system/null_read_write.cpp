#include "test.h"

const char* SRC_FILE_NAME = "data/test.txt";
const char* DST_FILE_NAME = "data/test_copy.txt";

int main ()
{
  printf ("Results of null_read_write_test:\n");

  try
  {
    InputFile  file1 (SRC_FILE_NAME);
    OutputFile file2 (DST_FILE_NAME);

    printf ("File::Read(0,0): %lu\n", file1.Read (0,0));
    printf ("File::Write(0,0): %lu\n", file2.Write (0,0));

    file2.Close ();

    FileSystem::Remove (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
