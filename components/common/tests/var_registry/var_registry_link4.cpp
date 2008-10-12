#include "shared.h"

void enumerator (const char* var_name, const VarRegistry& registry)
{
  printf ("%s=%s; ", var_name, to_string (registry.GetValue (var_name)).c_str ());
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

  for (size_t i = 0; i < sizeof (events) / sizeof (*events); i++)
  {
    VarRegistryEvent event = (VarRegistryEvent)i;

    registry.RegisterEventHandler (mask, event, xtl::bind (&notify, _1, event, mask, xtl::cref (registry)));
  }
}

int main ()
{
  printf ("Results of var_registry_link4_test:\n");

  try
  {
    VarRegistrySystem::Mount ("a.b", TestVarRegistry::Create ().get ());

    VarRegistrySystem::Link ("a.c", "a.b");

    VarRegistry registry ("a");

    register_handler ("*", registry);

    registry.SetValue ("c.x", xtl::any (stl::string ("x"), true));
    registry.SetValue ("b.y", xtl::any (stl::string ("y"), true));
    registry.SetValue ("b.x.y", xtl::any (stl::string ("x.y"), true));
    registry.SetValue ("c.xxxx.y", xtl::any (stl::string ("xxxx.y"), true));

    printf ("enumerate a vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));
    printf ("\n");

    registry.Open ("a.b");

    VarRegistrySystem::Unlink ("a.c");

    printf ("enumerate a.b vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));

    registry.Open ("a.c");
    printf ("\nenumerate a.c vars:\n");
    registry.EnumerateVars (xtl::bind (&enumerator, _1, xtl::cref (registry)));
    printf ("\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
