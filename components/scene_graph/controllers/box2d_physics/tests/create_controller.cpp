#include "shared.h"

int main ()
{
  printf ("Results of create_controller_test:\n");
  
  try
  {
    Scene scene;
    
    scene.Root ().AttachController ("Box2D.World");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
