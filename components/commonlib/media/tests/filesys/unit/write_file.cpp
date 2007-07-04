#include "test.h"

const char* SRC_FILE_NAME = "data/test.txt";
const char* DST_FILE_NAME = "data/test_copy.txt";

int main ()
{
  printf ("Results of write_file_test:\n");
  
  try
  {
    InputFile src_file (SRC_FILE_NAME);
    File      dst_file = OutputFile (DST_FILE_NAME);

    char buf [128];
        
    while (!src_file.Eof ())
      dst_file.Write (buf,src_file.Read (buf,sizeof (buf)));

    dst_file.Close ();

    PrintFileInfo (DST_FILE_NAME);

    FileSystem::Remove (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }    

  return 0;
}
