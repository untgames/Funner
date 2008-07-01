#include "shared.h"

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s; ", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

int main ()
{
  printf ("Results of var_registry1_test:\n");
  
  try
  {
    VarRegistrySystem::Mount ("test", TestVarRegistry::Create ().get ());
    
    VarRegistry registry ("test");
    
    registry.SetValue ("x", xtl::any (stl::string ("x"), true));
    registry.SetValue ("y", xtl::any (stl::string ("y"), true));
    registry.SetValue ("x.y", xtl::any (stl::string ("x.y"), true));
    registry.SetValue ("xxxx.y", xtl::any (stl::string ("xxxx.y"), true));    

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
