#include "shared.h"

bool check_exception (const char* message)
{
  printf ("check_exception(%s)\n", message);
  
  return true;
}

int main ()
{
  printf ("Results of scene_context3_test:\n");
  
  try
  {
    SceneContext context;
    
    context.RegisterErrorHandler ("error:*", &check_exception);
    
    printf ("check: %d\n", context.FilterError ("error: my error"));
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
