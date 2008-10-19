#include "shared.h"

using namespace render;
using namespace script;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* RENDER_RECT_LIBRARY     = "Render.Rect";
const char* RENDER_VIEWPORT_LIBRARY = "Render.Viewport";
const char* RENDER_SCREEN_LIBRARY   = "Render.Screen";

/*
    Утилиты
*/

///Получение параметров области экрана и области вывода
int    get_rect_left   (const Rect& rect)         { return rect.left; }
int    get_rect_top    (const Rect& rect)         { return rect.top; }
size_t get_rect_width  (const Rect& rect)         { return rect.width; }
size_t get_rect_height (const Rect& rect)         { return rect.height; }
int    get_rect_right  (const Rect& rect)         { return rect.left + rect.width; }
int    get_rect_bottom (const Rect& rect)         { return rect.top + rect.height; }
void   set_rect_left   (Rect& rect, int param)    { rect.left = param; }
void   set_rect_top    (Rect& rect, int param)    { rect.top = param; }
void   set_rect_width  (Rect& rect, size_t param) { rect.width = param; }
void   set_rect_height (Rect& rect, size_t param) { rect.height = param; }

///Создание прямоугольной области
Rect create_empty_rect ()
{
  return Rect ();
}

Rect create_rect (int left, int top, size_t width, size_t height)
{
  return Rect (left, top, width, height);
}

///Регистрация библиотеки работы с прямоугольными областями
void bind_rect_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (RENDER_RECT_LIBRARY);
  
    //регистрация функции создания
    
  lib.Register ("Create", make_invoker (make_invoker (&create_rect), make_invoker (&create_empty_rect)));
  
    //регистрация операций
    
  lib.Register ("get_Left",    make_invoker (&get_rect_left));
  lib.Register ("get_Top",     make_invoker (&get_rect_top));
  lib.Register ("get_Right",   make_invoker (&get_rect_right));
  lib.Register ("get_Bottom",  make_invoker (&get_rect_bottom));  
  lib.Register ("get_Width",   make_invoker (&get_rect_width));
  lib.Register ("get_Height",  make_invoker (&get_rect_height));
  lib.Register ("set_Left",    make_invoker (&set_rect_left));
  lib.Register ("set_Top",     make_invoker (&set_rect_top));
  lib.Register ("set_Width",   make_invoker (&set_rect_width));
  lib.Register ("set_Height",  make_invoker (&set_rect_height));  
  
  environment.RegisterType<Rect> (RENDER_RECT_LIBRARY);
}

///Создание вьюпорта
Viewport create_viewport ()
{
  return Viewport ();
}

///Регистрация библиотеки работы с вьюпортами
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
  lib.Register ("get_BackgroundState", make_invoker (&Viewport::BackgroundState));
  lib.Register ("get_Id",              make_invoker (&Viewport::Id));
  lib.Register ("get_Area",            make_invoker (&Viewport::Area));
  lib.Register ("set_Area",            make_invoker (implicit_cast<const Rect& (Viewport::*)() const> (&Viewport::Area)));

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

///Создание экрана
Screen create_screen ()
{
  return Screen ();
}

///Регистрация библиотеки работы с рабочими столами
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
  lib.Register ("get_BackgroundState", make_invoker (&Screen::BackgroundState));
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
  
  bind_rect_library     (environment);
  bind_viewport_library (environment);
  bind_screen_library   (environment);
}

}

}
