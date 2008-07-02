#include "shared.h"

const char* configuration_file_name_mask = "data/configuration*";

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s\n", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

int main ()
{
  printf ("Results of var_registry_xml_cfg2_test:\n");
  
  try
  {
    VarRegistrySystem::Mount ("etc", TestVarRegistry::Create ().get ());
    
    VarRegistry registry ("etc");
    
    load_xml_configurations (registry, configuration_file_name_mask);

    printf ("enumerate all vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
