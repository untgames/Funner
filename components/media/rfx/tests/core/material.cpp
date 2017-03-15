#include "shared.h"

int main ()
{
  printf ("Results of material_test:\n");
  
  try
  {
    Material material;
    
    dump ("default constructor", material);
    
    float float_array [] = {2.34f, 4.56f};
    
    material.Properties ().SetProperty ("String", "Hello world");
    material.Properties ().SetProperty ("Integer", 1);
    material.Properties ().SetProperty ("Float", sizeof (float_array) / sizeof (*float_array), float_array);
    material.Properties ().SetProperty ("Vector", vec4f (1.0f, 2.0f, 3.0f, 4.0f));
    
    material.SetProgram ("my_effect");
    material.SetBlendMode ("my_blend_mode");
    material.SetName ("material1");

    Texmap texmap;
    
    texmap.SetImage ("image.png");
    texmap.SetSemantic ("diffuse");
    
    material.AddTexmap (texmap);

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
