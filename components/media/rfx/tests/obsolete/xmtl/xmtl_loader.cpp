#include "shared.h"

const char* SRC_LIBRARY_FILE_NAME = "data/test.xmtl";
const char* DST_LIBRARY_FILE_NAME = "/io/stdout/test.xmtl";

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    printf ("Results of xmtl_loader_test:\n");
       
    MaterialLibrary (SRC_LIBRARY_FILE_NAME, &print).Save (DST_LIBRARY_FILE_NAME, &print);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
