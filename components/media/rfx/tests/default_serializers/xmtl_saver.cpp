#include "shared.h"

const char* LIBRARY_NAME      = "data/test.xmtl";
const char* OUTPUT_FILE_NAME  = "/io/stdout/test.xmtl";

int main ()
{
  printf ("Results of xmtl_saver_test:\n");
  
  try
  {
    MaterialLibrary (LIBRARY_NAME).Save (OUTPUT_FILE_NAME);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
