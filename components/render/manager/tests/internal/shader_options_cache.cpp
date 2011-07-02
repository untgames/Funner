#include "shared.h"

void print_options (const char* name, const ShaderOptions& options)
{
  printf ("%s options: '%s' (hash=%08x)\n", name, options.options.c_str (), options.options_hash);
}

int main ()
{
  printf ("Results of shader_options_cache_test:\n");
  
  try
  {
    common::PropertyMap properties;
    
    properties.SetProperty ("DefineA", 1);
    properties.SetProperty ("DefineC", "test");    
    
    ShaderOptionsCache cache;

    cache.SetProperties (properties);
    
    ShaderOptionsLayout list1;
    
    list1.Add ("DefineA");
    list1.Add ("DefineB");
    list1.Add ("DefineC");

    print_options ("List1", cache.GetShaderOptions (list1));
    
    ShaderOptionsLayout list2;
    
    list2.Add ("DefineA");
    list2.Add ("DefineC");
    
    print_options ("List2", cache.GetShaderOptions (list2));
    
    printf ("Cached layouts count: %u\n", cache.CachedLayoutsCount ());
    printf ("List1 builders count: %u\n", list1.CachedShaderBuildersCount ());    
    printf ("List2 builders count: %u\n", list2.CachedShaderBuildersCount ());    
    
    printf ("change existring property\n");    
    
    properties.SetProperty ("DefineA", 2);
    
    print_options ("List1", cache.GetShaderOptions (list1));    
    print_options ("List2", cache.GetShaderOptions (list2));
    
    printf ("Cached layouts count: %u\n", cache.CachedLayoutsCount ());
    printf ("List1 builders count: %u\n", list1.CachedShaderBuildersCount ());    
    printf ("List2 builders count: %u\n", list2.CachedShaderBuildersCount ());    
        
    printf ("add new property\n");
    
    properties.SetProperty ("DefineB", 3.14f);
    
    print_options ("List1", cache.GetShaderOptions (list1));    
    print_options ("List2", cache.GetShaderOptions (list2));
    
    printf ("Cached layouts count: %u\n", cache.CachedLayoutsCount ());
    printf ("List1 builders count: %u\n", list1.CachedShaderBuildersCount ());    
    printf ("List2 builders count: %u\n", list2.CachedShaderBuildersCount ());        
    
    printf ("change list\n");
    
    list1.Clear ();
    
    print_options ("List1", cache.GetShaderOptions (list1));        
    
    printf ("Cached layouts count: %u\n", cache.CachedLayoutsCount ());
    printf ("List1 builders count: %u\n", list1.CachedShaderBuildersCount ());    
    printf ("List2 builders count: %u\n", list2.CachedShaderBuildersCount ());

    printf ("remove all properties\n");

    properties.Clear ();    
    
    print_options ("List1", cache.GetShaderOptions (list1));    
    print_options ("List2", cache.GetShaderOptions (list2));
    
    printf ("Cached layouts count: %u\n", cache.CachedLayoutsCount ());
    printf ("List1 builders count: %u\n", list1.CachedShaderBuildersCount ());    
    printf ("List2 builders count: %u\n", list2.CachedShaderBuildersCount ());            
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
