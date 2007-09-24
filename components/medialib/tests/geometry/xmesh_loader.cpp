#include "shared.h"

const char* SRC_MODEL_FILE_NAME = "data/test.xmesh";
const char* DST_MODEL_FILE_NAME = "/io/stdout/test.xmesh";

int main ()
{
  try
  {
    printf ("Results of xmesh_loader_test:\n");
    
    MeshModel (SRC_MODEL_FILE_NAME).Save (DST_MODEL_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
