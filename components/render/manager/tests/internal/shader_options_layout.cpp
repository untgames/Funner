#include "shared.h"

int main ()
{
  printf ("Results of shader_options_layout_test:\n");
  
  try
  {
    ShaderOptionsLayout list;
    
    list.Add ("DefineA");
    list.Add ("DefineB");    
    list.Add ("DefineC");
    
    common::PropertyMap properties1;
    
    properties1.SetProperty ("DefineA", 1);
    properties1.SetProperty ("DefineC", "test");    
    
    ShaderOptions options;
    
    list.GetShaderOptions (properties1, options);
    
    printf ("Options: '%s' (hash=%08x)\n", options.options.c_str (), options.options_hash);
    
    common::PropertyMap properties2;
    
    properties2.SetProperty ("DefineB", 3.14f);
    properties2.SetProperty ("DefineC", math::vec4f (1.0f, 2.0f, 3.0f, 4.0f));
    
    list.GetShaderOptions (properties2, options);
    
    printf ("Options: '%s' (hash=%08x)\n", options.options.c_str (), options.options_hash);    
    
    common::PropertyMap properties3;
    
    properties3.SetProperty ("DefineA", 2);
    properties3.SetProperty ("DefineC", "test2");

    list.GetShaderOptions (properties3, options);            
    
    printf ("Options: '%s' (hash=%08x)\n", options.options.c_str (), options.options_hash);        
    
    printf ("Builders count: %u\n", list.CachedShaderBuildersCount ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
