#include "shared.h"

typedef com_ptr<VisualModel> ModelPtr;

int main ()
{
  printf ("Results of visual_model_basic_test:\n");
  
  ModelPtr model (VisualModel::Create (), false);

  model->SetMeshName ("mesh1.mesh");
  
  printf ("Mesh name: '%s'\n", model->MeshName ());
  
    //проверка компилируемости
  
  model->SetInfiniteBounds ();
  model->SetBoundBox (aaboxf (-10.0f, 10.0f));

  return 0;
}
