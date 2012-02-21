#include "shared.h"

using namespace scene_graph;

int main ()
{
  printf ("Results of scene_context2_test:\n");
  
  try
  {
    SceneContext context;
    
    context.Properties ().SetProperty ("A", "B");
    
    printf ("properties count: %u\n", context.Properties ().Size ());
    
    context.Clear ();
    
    printf ("properties count: %u\n", context.Properties ().Size ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
