#include "shared.h"

using namespace common;
using namespace xtl;

namespace
{

const char* COMMON_VAR_REGISTRY_LIBRARY = "VarRegistry";

class DynamicCastRootVarRegistry : public xtl::reference_counter, public xtl::dynamic_cast_root
{
  public:
    typedef xtl::intrusive_ptr<DynamicCastRootVarRegistry> Pointer;

    static Pointer Create ()
    {
      return Pointer (new DynamicCastRootVarRegistry, false);
    }

    const char* BranchName ()
    {
      return registry.BranchName ();
    }

    const char* GetValue (const char* var_name) const
    {
      return to_string (registry.GetValue (var_name)).c_str ();
    }

    void SetValue (const char* var_name, const char* value)
    {
      stl::string str_value (value);

      registry.SetValue (var_name, value);
    }

    bool HasVariable (const char* var_name)
    {
      return registry.HasVariable (var_name);
    }

    bool IsOpened ()
    {
      return registry.IsOpened ();
    }

    void Open (const char* branch_name)
    {
      registry.Open (branch_name);
    }

    void Close ()
    {
      registry.Close ();
    }

  private:
    VarRegistry registry;
};

}

namespace script
{

namespace binds
{

/*
    Регистрация библиотеки работы с деревьями строк
*/

void bind_common_var_registry (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (COMMON_VAR_REGISTRY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&DynamicCastRootVarRegistry::Create));

    //регистрация операций

  lib.Register ("get_BranchName", make_invoker (&DynamicCastRootVarRegistry::BranchName));

  lib.Register ("GetValue",           make_invoker (&DynamicCastRootVarRegistry::GetValue));
  lib.Register ("SetValue",           make_invoker (&DynamicCastRootVarRegistry::SetValue));
  lib.Register ("HasVariable",        make_invoker (&DynamicCastRootVarRegistry::HasVariable));
  lib.Register ("IsOpened",           make_invoker (&DynamicCastRootVarRegistry::IsOpened));
  lib.Register ("Open",               make_invoker (&DynamicCastRootVarRegistry::Open));
  lib.Register ("Close",              make_invoker (&DynamicCastRootVarRegistry::Close));

    //регистрация типов данных

  environment.RegisterType<DynamicCastRootVarRegistry> (COMMON_VAR_REGISTRY_LIBRARY);
}

}

}

