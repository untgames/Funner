#include "shared.h"

using namespace render;
using namespace script;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* RENDER_VIEWPORT_LIBRARY = "Render.Viewport";
const char* RENDER_SCREEN_LIBRARY   = "Render.Screen";

/*
    Создание вьюпорта
*/

Viewport create_viewport ()
{
  return Viewport ();
}

/*
   Регистрация библиотеки работы с вьюпортами
*/

void bind_viewport_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RENDER_VIEWPORT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_viewport));

    //регистрация операций

  lib.Register ("set_Name",            make_invoker (&Viewport::SetName));
  lib.Register ("get_Name",            make_invoker (&Viewport::Name));
  lib.Register ("set_RenderPath",      make_invoker (&Viewport::SetRenderPath));
  lib.Register ("get_RenderPath",      make_invoker (&Viewport::RenderPath));
  lib.Register ("set_ZOrder",          make_invoker (&Viewport::SetZOrder));
  lib.Register ("get_ZOrder",          make_invoker (&Viewport::ZOrder));
  lib.Register ("set_Camera",          make_invoker (&Viewport::SetCamera));
  lib.Register ("get_Camera",          make_invoker (implicit_cast<scene_graph::Camera* (Viewport::*) ()> (&Viewport::Camera)));
  lib.Register ("set_Active",          make_invoker (&Viewport::SetActive));
  lib.Register ("get_Active",          make_invoker (&Viewport::IsActive));
  lib.Register ("set_BackgroundColor", make_invoker (implicit_cast<void (Viewport::*) (const math::vec4f&)> (&Viewport::SetBackgroundColor)));
  lib.Register ("get_BackgroundColor", make_invoker (&Viewport::BackgroundColor));
  lib.Register ("set_BackgroundState", make_invoker (&Viewport::SetBackgroundState));
  lib.Register ("get_HasBackground",   make_invoker (&Viewport::HasBackground));
  lib.Register ("get_Id",              make_invoker (&Viewport::Id));

  lib.Register ("SetArea",             make_invoker (implicit_cast<void (Viewport::*) (int, int, size_t, size_t)> (&Viewport::SetArea)));
  lib.Register ("SetOrigin",           make_invoker (&Viewport::SetOrigin));
  lib.Register ("SetSize",             make_invoker (&Viewport::SetSize));
  lib.Register ("Activate",            make_invoker (&Viewport::Activate));
  lib.Register ("Deactivate",          make_invoker (&Viewport::Deactivate));
  lib.Register ("SetBackgroundColor",  make_invoker (make_invoker (implicit_cast<void (Viewport::*) (float, float, float, float)> (&Viewport::SetBackgroundColor)),
                 make_invoker<void (Viewport&, float, float, float)> (bind (implicit_cast<void (Viewport::*) (float, float, float, float)> (&Viewport::SetBackgroundColor), _1, _2, _3, _4, 0.f))));
  lib.Register ("EnableBackground",    make_invoker (&Viewport::EnableBackground));
  lib.Register ("DisableBackground",   make_invoker (&Viewport::DisableBackground));
  lib.Register ("SetProperty",         make_invoker (&Viewport::SetProperty));
  lib.Register ("GetProperty",         make_invoker (&Viewport::GetProperty));
  lib.Register ("HasProperty",         make_invoker (&Viewport::HasProperty));
  lib.Register ("RemoveProperty",      make_invoker (&Viewport::RemoveProperty));
  lib.Register ("RemoveAllProperties", make_invoker (&Viewport::RemoveAllProperties));

  environment.RegisterType<Viewport> (RENDER_VIEWPORT_LIBRARY);
}

/*
    Создание рабочего стола
*/

Screen create_screen ()
{
  return Screen ();
}

/*
   Регистрация библиотеки работы с рабочими столами
*/

void bind_screen_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RENDER_SCREEN_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_screen));

    //регистрация операций

  lib.Register ("set_Name",            make_invoker (&Screen::SetName));
  lib.Register ("get_Name",            make_invoker (&Screen::Name));
  lib.Register ("set_BackgroundColor", make_invoker (implicit_cast<void (Screen::*) (const math::vec4f&)> (&Screen::SetBackgroundColor)));
  lib.Register ("get_BackgroundColor", make_invoker (&Screen::BackgroundColor));
  lib.Register ("set_BackgroundState", make_invoker (&Screen::SetBackgroundState));
  lib.Register ("get_HasBackground",   make_invoker (&Screen::HasBackground));
  lib.Register ("get_Id",              make_invoker (&Screen::Id));
  lib.Register ("get_ViewportsCount",  make_invoker (&Screen::ViewportsCount));

  lib.Register ("SetBackgroundColor",  make_invoker (make_invoker (implicit_cast<void (Screen::*) (float, float, float, float)> (&Screen::SetBackgroundColor)),
                 make_invoker<void (Screen&, float, float, float)> (bind (implicit_cast<void (Screen::*) (float, float, float, float)> (&Screen::SetBackgroundColor), _1, _2, _3, _4, 0.f))));
  lib.Register ("EnableBackground",    make_invoker (&Screen::EnableBackground));
  lib.Register ("DisableBackground",   make_invoker (&Screen::DisableBackground));
  lib.Register ("Attach",              make_invoker (&Screen::Attach));
  lib.Register ("Detach",              make_invoker (&Screen::Detach));
  lib.Register ("DetachAllViewports",  make_invoker (&Screen::DetachAllViewports));
  lib.Register ("Viewport",            make_invoker (implicit_cast<Viewport& (Screen::*) (size_t)> (&Screen::Viewport)));

  environment.RegisterType<Screen> (RENDER_SCREEN_LIBRARY);
}

}

namespace script
{

namespace binds
{

/*
    Регистрация библиотеки рендера
*/

void bind_render_library (Environment& environment)
{
    //регистрация библиотек
  
  bind_viewport_library (environment);
  bind_screen_library   (environment);
}

}

}
