#include "shared.h"

const char* CONFIGURATION_FILE_NAME = "data/auto_cfg2.xml";

void enumerator (const char* var_name, VarRegistry& registry)
{
  printf ("Variable '%s' value is '%s'\n", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

void log_handler (const char* log_name, const char* event)
{
  printf ("Log '%s' message: '%s'\n", log_name, event);
}

int main ()
{
  printf ("Results of configuration_registry2_test:\n");

  try
  {   
    LogFilter filter ("*", &log_handler);    
    
    ConfigurationRegistry::LoadConfiguration (CONFIGURATION_FILE_NAME);

    VarRegistry registry ("Configuration.Windows");

    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::ref (registry)));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
