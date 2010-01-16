#include "shared.h"

int main ()
{
  printf ("Results of material_test:\n");
  
  try
  {
    Material material;
    
    dump ("default constructor", material);
    
    material.Properties ().SetProperty ("String", "Hello world");
    material.Properties ().SetProperty ("Integer", 1);
    material.Properties ().SetProperty ("Float", 2.34f);
    material.Properties ().SetProperty ("Vector", vec4f (1.0f, 2.0f, 3.0f, 4.0f));
    
    material.SetEffect ("my_effect");
    material.SetName ("material1");
    
    dump ("after change properties", material);
    
    Material clone_material = material.Clone ();
    
    clone_material.Properties ().SetProperty ("Matrix", mat4f (3.0f));
    
    dump ("clone affects", material);
    dump ("clone", clone_material);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
