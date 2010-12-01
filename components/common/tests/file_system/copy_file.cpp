#include <common/file.h>
#include <stdio.h>
#include <string.h>

using namespace common;

int main ()
{
  printf ("Results of copy_file_test:\n");
  
  try
  {
    FileSystem::CopyFile ("data/test.txt", "/io/stdout");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
