#include <common/file.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of symbolic_link_test:\n");
  
  try
  {
    FileSystem::Mount ("/my", "data");
    
    FileSystem::CopyFile ("/my/test.txt", "/io/stdout");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
