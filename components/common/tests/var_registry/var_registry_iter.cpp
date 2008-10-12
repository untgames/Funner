#include "shared.h"

int main ()
{
  printf ("Results of var_registry_iter_test:\n");

  try
  {
    TestVarRegistry::Pointer test_var_registry = TestVarRegistry::Create ();

    VarRegistrySystem::Mount ("test", test_var_registry.get ());

    VarRegistry registry ("");

    registry.SetValue ("test.x", xtl::any (stl::string ("x"), true));
    registry.SetValue ("test.x.y", xtl::any (stl::string ("x"), true));
    registry.SetValue ("test.x.y.z", xtl::any (stl::string ("x"), true));
    registry.SetValue ("test.x.z", xtl::any (stl::string ("x"), true));

    VarRegistry registry1 ("test.x");

    printf ("First enumeration:\n");
    for (VarRegistry::Iterator iter=registry1.CreateIterator (); iter; ++iter)
      printf ("name='%s' vars_count=%lu\n", iter->Name (), iter->VarsCount ());

    registry.SetValue ("test.x.z.x", xtl::any (stl::string ("x"), true));

    test_var_registry->RemoveVariable ("x.y");

    printf ("Second enumeration:\n");
    for (VarRegistry::Iterator iter=registry1.CreateIterator (); iter; ++iter)
      printf ("name='%s' vars_count=%lu\n", iter->Name (), iter->VarsCount ());
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
