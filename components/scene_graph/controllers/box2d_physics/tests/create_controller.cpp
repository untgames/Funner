#include "shared.h"

int main ()
{
  printf ("Results of create_controller_test:\n");
  
  try
  {
    Scene scene;
    
    scene.Root ().AttachController ("Box2D.World");
    
    Body::Pointer body = Body::Create ();
    
    body->SetName ("body1");
    body->SetModelName ("shape1");
    
    body->BindToScene (scene);    
    
    scene.Root ().Update (1.0f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
