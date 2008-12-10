#include "shared.h"

int main ()
{
  printf ("Results of create_controller_test:\n");    
  
  try
  {
    common::LogFilter log_filter ("*", &print_log);
    
    Scene scene;
    
    physics::ControllerDesc desc;
    
    desc.world_bound_box    = bound_volumes::aaboxf (math::vec3f (-100.0f), math::vec3f (100.0f));
    desc.configuration_name = "data/test.xphys";

    scene.Root ().AttachController ("Box2D.World", desc);
    
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
