#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/file.h>

using namespace common;

void remove_all ()
{
  system ("rm -rf results");
}

int main ()
{
  printf ("Results of mkdir_test:\n");
  
  try
  {
    remove_all ();
    
    FileSystem::Mkdir ("results/data/");
    FileSystem::Mkdir ("results/a/b/c/d");
    
    printf ("isdir: %s\n", FileSystem::IsDir ("results/a/b/c/d") ? "true" : "false");
    
    remove_all ();
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
