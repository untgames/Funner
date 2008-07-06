#include "shared.h"

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s; ", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

int main ()
{
  printf ("Results of var_registry6_test:\n");
  
  try
  {
    VarRegistrySystem::Mount ("test", TestVarRegistry::Create ().get ());
    VarRegistrySystem::Mount ("a", TestVarRegistry::Create ().get ());
    
    VarRegistry registry ("");
    
    registry.SetValue ("test.x", xtl::any (stl::string ("x"), true));

    VarRegistry registry2 ("a.b");
    
    registry2.SetValue ("x", xtl::any (stl::string ("x"), true));

    printf ("enumerate all vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
