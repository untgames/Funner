#include "shared.h"

using namespace media;

const char* file_name = "data/atlas.xatlas";
const char* RESULT_FILE_NAME = "/io/stdout/atlas.xatlas";

int main ()
{
  printf ("Results of xatlas1 test:\n");

  try
  {
    Atlas (file_name).Save (RESULT_FILE_NAME);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ());
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
