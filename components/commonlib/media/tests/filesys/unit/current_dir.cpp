#include <common/file.h>
#include <stdio.h>

using namespace common;

int main ()
{
  printf ("Results of current_dir_test:\n");  

  try
  {
    FileSystem::SetCurrentDir ("data");
    FileSystem::SetCurrentDir ("search/../search/.././");
    
    printf ("Current dir: '%s'\n",FileSystem::GetCurrentDir ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }    

  return 0;
}
