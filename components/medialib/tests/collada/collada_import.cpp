#include "shared.h"

using namespace media::geometry;

const char* COLLADA_FILE_NAME = "data/test.dae";
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
    printf ("Load collada model '%s'", COLLADA_FILE_NAME);
    
    media::collada::Model collada_model (COLLADA_FILE_NAME, &print);
    
    printf ("Importing...\n");
    
    MeshModel mesh_model;
    
    append (collada_model, mesh_model);
    
    printf ("Save XMesh model '%s'\n", DST_FILE_NAME);
    
    mesh_model.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
