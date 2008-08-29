#include "shared.h"

const char* configuration_file_name = "data/auto_cfg.xml";

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s\n", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

int main ()
{
  printf ("Results of var_registry_xml_cfg3_test:\n");
  
  try
  {
    VarRegistrySystem::Mount ("etc", TestVarRegistry::Create ().get ());
    
    VarRegistry registry ("etc");
    
    load_xml_configuration (registry, configuration_file_name, "*");

    printf ("enumerate all vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
