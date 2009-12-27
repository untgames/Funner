#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
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
      engine::bind_common_file_library (environment);
      engine::bind_common_string_tree  (environment);
      engine::bind_common_signals      (environment);
      engine::bind_common_action_queue (environment);
      engine::bind_common_timer        (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> CommonScriptBind (COMPONENT_NAME);

}

}
