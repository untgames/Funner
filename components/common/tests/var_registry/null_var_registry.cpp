#include "shared.h"

void enumerator (const char* var_name)
{
  printf ("%s", var_name);
}

void notify (const char* var_name)
{
  printf ("notify %s\n", var_name);
}

int main ()
{
  printf ("Results of null_var_registry_test:\n");
  
  try
  {   
    VarRegistry registry;
    registry.Close ();
    
    registry.RegisterEventHandler ("*", VarRegistryEvent_OnCreateVar, &notify);

    registry.SetValue ("x", xtl::any (stl::string ("x"), true));
    registry.GetValue ("x");       

    printf ("has 'y': %s\n", registry.HasVariable ("y") ? "true" : "false");
    
    registry.EnumerateVars (&enumerator);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
