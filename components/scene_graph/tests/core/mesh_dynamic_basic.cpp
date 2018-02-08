#include "shared.h"

int main ()
{
  printf ("Results of mesh_dynamic_basic_test:\n");
  
  DynamicMesh::Pointer model (DynamicMesh::Create ());

  printf ("Mesh name: '%s'\n", model->Mesh ().Name ());
  
  media::geometry::Mesh new_mesh;

  new_mesh.Rename ("New mesh name");

  model->SetMesh (new_mesh);

  printf ("Mesh name: '%s'\n", model->Mesh ().Name ());

  //check code compilable
  
  model->SetInfiniteBounds ();
  model->SetBoundBox (aaboxf (-10.0f, 10.0f));

  return 0;
}
