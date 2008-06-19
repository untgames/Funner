#include "test.h"

const char* SRC_FILE_NAME = "data/test.txt";

int main ()
{
  printf ("Results of load_text_file_test:\n");
  
  try
  {
    stl::string s = FileSystem::LoadTextFile (SRC_FILE_NAME);
    
    printf ("file content:\n%s\n--end file content\n", s.c_str ());    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
