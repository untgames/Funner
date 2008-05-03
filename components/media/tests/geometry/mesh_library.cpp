#include "shared.h"

void dump (const MeshLibrary& library)
{
  printf ("library '%s' (%u meshes)\n", library.Name (), library.Size ());

  for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
    printf ("  mesh '%s'\n", i->Name ());
}

int main ()
{
  printf ("Results of mesh_library_test:\n");
  
  try
  {
    MeshLibrary library;
    
    library.Rename ("library1");
    
    const char* mesh_name [] = {"mesh1", "mesh2", "mesh3", "mesh4"};
    
    printf ("attach meshes\n");
    
    for (size_t i=0; i<4; i++)
    {
      Mesh mesh;

      mesh.Rename (mesh_name [i]);    

      library.Attach (mesh_name [i], mesh);
    }  
    
    dump (library);
    
    printf ("detach mesh\n");
    
    library.Detach ("mesh3");
    
    dump (library);
    
    printf ("clear library\n");
    
    library.Clear ();
    
    dump (library);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
