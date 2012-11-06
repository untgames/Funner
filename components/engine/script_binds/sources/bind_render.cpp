#include "shared.h"

#include <render/scene_render.h>

using namespace script;
using namespace render::obsolete;

namespace components
{

namespace render_bind
{

/*
    Константы
*/

const char* RENDER_TARGET_LIBRARY = "Render.RenderTarget";
const char* COMPONENT_NAME        = "script.binds.Render";
const char* BINDER_NAME           = "Render";

void bind_render_target_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (RENDER_TARGET_LIBRARY);

    //регистрация операций
    
  lib.Register ("CaptureImage", make_invoker (
    make_invoker (xtl::implicit_cast<void (RenderTarget::*)(media::Image&)> (&RenderTarget::CaptureImage)),
    make_invoker (xtl::implicit_cast<void (RenderTarget::*)(const char*)> (&RenderTarget::CaptureImage))
  ));

  lib.Register ("get_Screen", make_invoker (&RenderTarget::Screen));
  lib.Register ("set_Screen", make_invoker (&RenderTarget::SetScreen));

    //регистрация типов данных

  environment.RegisterType<RenderTarget> (RENDER_TARGET_LIBRARY);  
}

void bind_render_library (Environment& environment)
{
  bind_render_target_library (environment);
}

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
      bind_render_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> RenderScriptBind (COMPONENT_NAME);

}

}

}
