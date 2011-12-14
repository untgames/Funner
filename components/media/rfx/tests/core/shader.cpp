#include "shared.h"

int main ()
{
  printf ("Results of shader_test:\n");
  
  try
  {
    Shader shader;
    
    dump ("default constructor", shader);
    
    shader.SetName ("shader1");
    shader.SetProfile ("my_profile");
    shader.SetSourceCode ("hello world");
    
    dump ("after change properties", shader);
    
    Shader shader_clone = shader.Clone ();
    
    shader_clone.SetName ("test");
    
    dump ("clone not affects", shader);
    dump ("clone", shader_clone);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
