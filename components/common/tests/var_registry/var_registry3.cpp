#include "shared.h"

void test_vars (const VarRegistry& registry)
{
  const char* var_names [] = {"x", "xxxx.y", "y", "x.y"};

  for (int i=0; i<sizeof (var_names)/sizeof (*var_names); i++)
  {
    const char* name = var_names [i];
    
    if (registry.HasVariable (name))
      printf ("'%s'=%s\n", name, registry.GetValue (name));
    else
      printf ("variable '%s' not found\n", name);
  }
}

int main ()
{
  printf ("Results of var_registry3_test:\n");
  
  try
  {
    TestVarRegistry::Pointer test_registry = TestVarRegistry::Create ();

    VarRegistrySystem::Mount ("foo.bar", test_registry.get ());    
    
    VarRegistry registry;
    
    registry.Open ("foo");    

    registry.SetValue ("x", "x");
    registry.SetValue ("y", "y");
    registry.SetValue ("x.y", "x.y");
    registry.SetValue ("xxxx.y", "xxxx.y");    
    
    test_vars (registry);
    
    test_registry->RemoveVariable ("x");
    test_registry->RemoveVariable ("xxxx.y");
    
    printf ("after remove variables\n");
    
    test_vars (registry);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
