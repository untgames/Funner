#include "shared.h"

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
  printf ("Results of var_registry_link2_test:\n");

  try
  {
    TestVarRegistry::Pointer test_registry = TestVarRegistry::Create ();

    VarRegistrySystem::Mount ("test", test_registry.get ());

    VarRegistrySystem::Link ("link", "test");

    VarRegistry registry ("link");
    VarRegistry registry2 ("");

    register_handler ("*y*", registry);
    register_handler ("*", registry);

    registry.SetValue ("x", xtl::any (stl::string ("x"), true));
    registry.SetValue ("x", xtl::any (stl::string ("x"), true));
    registry.SetValue ("y", xtl::any (stl::string ("y"), true));
    registry.SetValue ("x.y", xtl::any (stl::string ("x.y"), true));
    registry.SetValue ("xxxx.y", xtl::any (stl::string ("xxxx.y"), true));

    test_registry->RemoveVariable ("x");
    test_registry->RemoveVariable ("xxxx.y");

    printf ("end scope\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
