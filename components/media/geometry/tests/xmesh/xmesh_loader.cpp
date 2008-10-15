#include "shared.h"

const char* LOG_MASK            = "media.geometry.*";
const char* SRC_MODEL_FILE_NAME = "data/test.xmesh";
const char* DST_MODEL_FILE_NAME = "/io/stdout/test.xmesh";

void print_log (const char*, const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    printf ("Results of xmesh_loader_test:\n");

    common::LogFilter log_filter (LOG_MASK, &print_log);

    MeshLibrary (SRC_MODEL_FILE_NAME).Save (DST_MODEL_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
