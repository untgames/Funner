#include "shared.h"

using namespace media::geometry;

const char* COLLADA_FILE_NAME = "../collada/data/ape.dae";
const char* DST_FILE_NAME     = "/io/stdout/test.xmesh";

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of collada_import_test:\n");
  
  try
  {
    printf ("Load collada model '%s'\n", COLLADA_FILE_NAME);
    
    media::collada::Model collada_model (COLLADA_FILE_NAME, &print);
    
    printf ("Converting...\n");
    
    MeshLibrary mesh_library;
    
    convert (collada_model, mesh_library);
    
    printf ("Save XMesh library '%s'\n", DST_FILE_NAME);
    
    mesh_library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
