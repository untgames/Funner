#include "shared.h"

int main ()
{
  printf ("Results of scene_root_unbind_test:\n");
  
  try
  {
    Scene scene;

    scene.SetName ("scene1");

    Node::Pointer node = Node::Create ();
    
    scene.Root ().BindToParent (*node);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
