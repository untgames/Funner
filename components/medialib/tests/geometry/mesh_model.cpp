#include "shared.h"

void dump (const MeshModel& model)
{
  printf ("model '%s' (%u meshes)\n", model.Name (), model.MeshesCount ());    

  for (size_t i=0; i<model.MeshesCount (); i++)
    printf ("  mesh '%s'\n", model.Mesh (i).Name ());
}

int main ()
{
  printf ("Results of mesh_model_test:\n");
  
  try
  {
    MeshModel model;
    
    model.Rename ("model1");
    
    const char* mesh_name [] = {"mesh1", "mesh2", "mesh3", "mesh4"};
    
    printf ("attach meshes\n");
    
    for (size_t i=0; i<4; i++)
    {
      Mesh mesh;

      mesh.Rename (mesh_name [i]);    

      model.Attach (mesh);
    }  
    
    dump (model);
    
    printf ("detach mesh\n");
    
    model.Detach (2);
    
    dump (model);
    
    printf ("clear model\n");
    
    model.Clear ();
    
    dump (model);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
