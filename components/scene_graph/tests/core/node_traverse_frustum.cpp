#include "shared.h"

void print (Node& node)
{
  printf ("node '%s'\n", node.Name ());
}

int main ()
{
  printf ("Results of camera_ortho_test:\n");
  
  OrthoCamera::Pointer camera (OrthoCamera::Create ());

  camera->SetLeft   (-10);
  camera->SetRight  (10);
  camera->SetBottom (-10);
  camera->SetTop    (10);
  camera->SetZNear  (-10);
  camera->SetZFar   (10);
  
  Scene scene;

  TestEntity::Pointer entity1 = TestEntity::Create (), entity2 = TestEntity::Create (), entity3 = TestEntity::Create ();
  
  entity1->SetName ("entity1");
  entity2->SetName ("entity2");  
  entity3->SetName ("entity3");  

  entity1->SetBoundBox (bound_volumes::aaboxf (15.f, 20.f));
  entity2->SetBoundBox (bound_volumes::aaboxf (5.f, 15.f));
  
  entity1->BindToScene (scene);
  entity2->BindToScene (scene);  
  entity3->BindToScene (scene);  
  
  printf ("traverse nodes in frustum:\n");
  
  scene.Traverse (camera->Frustum (), &print);
  
  camera->Translate (math::vec3f (-6.0f, 0.0f, 0.0f));
  
  printf ("traverse nodes in frustum after camera move:\n");
  
  scene.Traverse (camera->Frustum (), &print);  

  return 0;
}
