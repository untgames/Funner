#include "shared.h"

int main ()
{
  printf ("Results of visual_model_basic_test:\n");
  
  VisualModel::Pointer model (VisualModel::Create ());

  model->SetMeshName ("mesh1.mesh");
  
  printf ("Mesh name: '%s'\n", model->MeshName ());
  
    //проверка компилируемости
  
  model->SetInfiniteBounds ();
  model->SetBoundBox (aaboxf (-10.0f, 10.0f));

  return 0;
}
