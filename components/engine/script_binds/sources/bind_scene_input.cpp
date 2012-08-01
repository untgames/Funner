#include "shared.h"

using namespace script;
using namespace input;
using namespace scene_graph;

namespace
{

/*
    Константы
*/

const char* SCENE_STATIC_INPUT_TRANSFORM_SPACE = "Scene.InputTransformSpace";
const char* SCENE_INPUT_MANAGER_LIBRARY        = "Scene.InputManager";
const char* COMPONENT_NAME                     = "script.binds.SceneInput";
const char* BINDER_NAME                        = "SceneInput";

/*
    Бинды InputTransformSpace
*/

void bind_scene_input_transform_space_library (script::Environment& env)
{
  InvokerRegistry lib = env.CreateLibrary (SCENE_STATIC_INPUT_TRANSFORM_SPACE);

  lib.Register ("get_Screen",  make_const (InputTransformSpace_Screen));
  lib.Register ("get_Camera",  make_const (InputTransformSpace_Camera));
  lib.Register ("get_Default", make_const (InputTransformSpace_Default));
}

/*
    Бинды SceneInputManager
*/

SceneInputManager create_manager ()
{
  return SceneInputManager ();
}

void set_input_screen (SceneInputManager& manager, xtl::any& screen)
{
  if (screen.empty ()) manager.SetScreen (0);
  else                 manager.SetScreen (&screen.cast<Screen&> ());
}

void bind_scene_input_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCENE_INPUT_MANAGER_LIBRARY);

  lib.Register ("Create",             make_invoker (&create_manager));
  lib.Register ("Reset",              make_invoker (&SceneInputManager::Reset));
  lib.Register ("get_Screen",         make_invoker (xtl::implicit_cast<Screen* (SceneInputManager::*)()> (&SceneInputManager::Screen)));
  lib.Register ("set_Screen",         make_invoker (&set_input_screen));
  lib.Register ("get_TouchSize",      make_invoker (&SceneInputManager::TouchSize));
  lib.Register ("get_TouchSizeSpace", make_invoker (&SceneInputManager::TouchSizeSpace));
  lib.Register ("SetTouchSize",       make_invoker (
    make_invoker<void (SceneInputManager&, float, InputTransformSpace)> (xtl::bind (&SceneInputManager::SetTouchSize, _1, _2, _3)),
    make_invoker<void (SceneInputManager&, float)> (xtl::bind (&SceneInputManager::SetTouchSize, _1, _2, InputTransformSpace_Default))
  ));
  lib.Register ("ProcessEvent", make_invoker (&SceneInputManager::ProcessEvent));

  environment.RegisterType<SceneInputManager> (SCENE_INPUT_MANAGER_LIBRARY);
}

}

namespace components
{

namespace scene_input_script_binds
{

/*
     омпонент
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
      bind_scene_input_transform_space_library (environment);
      bind_scene_input_manager_library         (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SceneInputScriptBind (COMPONENT_NAME);

}

}

}
