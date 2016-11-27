#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace components
{

namespace common_script_bind
{

/*
    Константы (имена библиотек)
*/

const char* COMPONENT_NAME = "script.binds.Common";
const char* BINDER_NAME    = "Common";

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      engine::bind_common_action                (environment);
      engine::bind_common_background_copy_state (environment);
      engine::bind_common_file_library          (environment);
      engine::bind_common_string_tree           (environment);
      engine::bind_common_signals               (environment);
      engine::bind_common_timer                 (environment);
      engine::bind_common_async_library         (environment);
      engine::bind_common_property_map          (environment);
      engine::bind_common_crypto                (environment);
      engine::bind_common_base64                (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> CommonScriptBind (COMPONENT_NAME);

}

}

}
