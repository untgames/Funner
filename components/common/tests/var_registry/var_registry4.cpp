#include "shared.h"

void test (const char* mount_point)
{
  try
  {
    VarRegistry registry (mount_point);    

    registry.SetValue ("x", xtl::any (stl::string ("x"), true));
  }
  catch (std::exception& e)
  {
    printf ("mount point '%s' exception: %s\n", mount_point, e.what ());
  }
}

int main ()
{
  printf ("Results of var_registry4_test:\n");
  
  try
  {
    TestVarRegistry::Pointer registry1 = TestVarRegistry::Create ().get ();
    TestVarRegistry::Pointer registry2 = TestVarRegistry::Create ().get ();

    VarRegistrySystem::Mount ("registry_1:1", registry1.get ());
    VarRegistrySystem::Mount ("registry_1:2", registry1.get ());
    VarRegistrySystem::Mount ("registry_2",   registry2.get ());

    test ("registry_1:1");
    test ("registry_1:2");
    test ("registry_2");
    test ("registry_3");

    VarRegistrySystem::Unmount ("registry_2");
    test ("registry_2");

    VarRegistrySystem::Unmount ("registry_1:1", registry1.get ());
    test ("registry_1:1");
    test ("registry_1:2");

    VarRegistrySystem::Mount ("registry_1:1", registry1.get ());
    VarRegistrySystem::Mount ("registry_2", registry2.get ());
    VarRegistrySystem::UnmountAll (registry1.get ());
    test ("registry_1:1");
    test ("registry_1:2");
    test ("registry_2");
  
    VarRegistrySystem::Mount ("registry_1:1", registry1.get ());
    VarRegistrySystem::UnmountAll ();
    test ("registry_1:1");
    test ("registry_2");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
