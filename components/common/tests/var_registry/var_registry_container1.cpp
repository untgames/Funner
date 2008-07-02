#include "shared.h"

void string_enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s; ", var_name, to_string (registry.GetValue (var_name)).c_str ());
}

void size_t_enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%u; ", var_name, registry.GetValue (var_name).cast<size_t> ());
}

void notify (const char* var_name, VarRegistryEvent event_id, const char* var_mask, const VarRegistry& registry)
{
  switch (event_id)
  {
    case VarRegistryEvent_OnCreateVar:
      printf ("create '%s'", var_name);
      break;
    case VarRegistryEvent_OnChangeVar:
      printf ("change '%s'='%s'", var_name, to_string (registry.GetValue (var_name)).c_str ());
      break;
    case VarRegistryEvent_OnDeleteVar:
      printf ("delete '%s'", var_name);
      break;
    default:
      return;
  }

  printf (" (mask='%s')\n", var_mask);
}

void register_handler (const char* mask, const VarRegistry& registry)
{
  VarRegistryEvent events [] = {VarRegistryEvent_OnCreateVar, VarRegistryEvent_OnChangeVar, VarRegistryEvent_OnDeleteVar};

  for (int i=0; i<sizeof (events)/sizeof (*events); i++)
  {
    VarRegistryEvent event = (VarRegistryEvent)i;
    
    registry.RegisterEventHandler (mask, event, xtl::bind (&notify, _1, event, mask, xtl::cref (registry)));
  }
}

int main ()
{
  printf ("Results of var_registry_container1_test:\n");
  
  try
  {
    VarRegistryContainer<stl::string> string_container;
    VarRegistryContainer<size_t> size_t_container;
    VarRegistryContainer<size_t> size_t_container2 (size_t_container);

    string_container.Mount ("string_branch");
    size_t_container2.Mount ("size_t_branch");

    VarRegistry string_registry ("string_branch");
    VarRegistry size_t_registry ("size_t_branch");
    
    register_handler ("*", string_registry);
    register_handler ("*", size_t_registry);

    string_registry.SetValue ("x", xtl::any (stl::string ("x"), true));
    string_registry.SetValue ("x.y", xtl::any (stl::string ("x.y"), true));
    size_t_registry.SetValue ("x123", xtl::any (123u, true));
    size_t_registry.SetValue ("x123", xtl::any (123u, true));
    size_t_registry.SetValue ("x.y99999", xtl::any (99999u, true));

    printf ("enumerate string vars:\n");
    string_registry.EnumerateVars (xtl::bind (&string_enumerator, _1, xtl::cref (string_registry)));
    printf ("\n");

    printf ("enumerate size_t vars:\n");
    size_t_registry.EnumerateVars (xtl::bind (&size_t_enumerator, _1, xtl::cref (size_t_registry)));
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
