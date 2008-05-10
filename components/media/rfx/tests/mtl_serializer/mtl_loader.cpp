#include "shared.h"

const char* SRC_LIBRARY_FILE_NAME = "data/test.mtl";
const char* DST_LIBRARY_FILE_NAME = "/io/stdout/test.mtl";

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    printf ("Results of mtl_loader_test:\n");
    
    MaterialLibraryManager::SetDebugLog (&print);
    
    MaterialLibrary (SRC_LIBRARY_FILE_NAME).Save (DST_LIBRARY_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
