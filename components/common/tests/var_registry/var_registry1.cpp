#include "shared.h"

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s; ", var_name, registry.GetValue (var_name));
}

int main ()
{
  printf ("Results of var_registry1_test:\n");
  
  try
  {
    VarRegistrySystem::Mount ("test", TestVarRegistry::Create ().get ());
    
    VarRegistry registry ("test");
    
    registry.SetValue ("x", "x");
    registry.SetValue ("y", "y");
    registry.SetValue ("x.y", "x.y");
    registry.SetValue ("xxxx.y", "xxxx.y");    

    printf ("enumerate all vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));

    printf ("\nenumerate 'x*' vars:\n");
    registry.EnumerateVars ("x*", xtl::bind (&enumerator, _1, xtl::cref (registry)));
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
