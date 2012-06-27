#include "shared.h"

#include <sg/camera.h>
#include <sg/screen.h>
#include <sg/viewport.h>

using namespace scene_graph;
using namespace script;
using namespace xtl;

namespace engine
{

namespace scene_graph_script_binds
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
int    get_rect_left   (const Rect& rect)         { return rect.x; }
int    get_rect_top    (const Rect& rect)         { return rect.y; }
size_t get_rect_width  (const Rect& rect)         { return rect.width; }
size_t get_rect_height (const Rect& rect)         { return rect.height; }
int    get_rect_right  (const Rect& rect)         { return rect.right (); }
int    get_rect_bottom (const Rect& rect)         { return rect.bottom (); }
void   set_rect_left   (Rect& rect, int param)    { rect.x      = param; }
void   set_rect_top    (Rect& rect, int param)    { rect.y      = param; }
void   set_rect_width  (Rect& rect, size_t param) { rect.width  = param; }
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
  InvokerRegistry lib = environment.CreateLibrary (RENDER_RECT_LIBRARY);
  
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
  InvokerRegistry lib = environment.CreateLibrary (RENDER_VIEWPORT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_viewport));

    //регистрация операций

  lib.Register ("set_Name",            make_invoker (&Viewport::SetName));
  lib.Register ("get_Name",            make_invoker (&Viewport::Name));
  lib.Register ("set_Technique",       make_invoker (&Viewport::SetTechnique));
  lib.Register ("get_Technique",       make_invoker (&Viewport::Technique));
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
  lib.Register ("set_Area",            make_invoker (implicit_cast<void (Viewport::*) (const Rect&)> (&Viewport::SetArea)));

  lib.Register ("SetArea",             make_invoker (implicit_cast<void (Viewport::*) (int, int, int, int)> (&Viewport::SetArea)));
  lib.Register ("SetOrigin",           make_invoker (&Viewport::SetOrigin));
  lib.Register ("SetSize",             make_invoker (&Viewport::SetSize));
  lib.Register ("get_MinDepth",        make_invoker (&Viewport::MinDepth));
  lib.Register ("get_MaxDepth",        make_invoker (&Viewport::MaxDepth));
  lib.Register ("set_MinDepth",        make_invoker (&Viewport::SetMinDepth));
  lib.Register ("set_MaxDepth",        make_invoker (&Viewport::SetMaxDepth));
  lib.Register ("SetDepthRange",       make_invoker (&Viewport::SetDepthRange));  
  lib.Register ("Activate",            make_invoker (&Viewport::Activate));
  lib.Register ("Deactivate",          make_invoker (&Viewport::Deactivate));
  lib.Register ("SetBackgroundColor",  make_invoker (make_invoker (implicit_cast<void (Viewport::*) (float, float, float, float)> (&Viewport::SetBackgroundColor)),
                 make_invoker<void (Viewport&, float, float, float)> (bind (implicit_cast<void (Viewport::*) (float, float, float, float)> (&Viewport::SetBackgroundColor), _1, _2, _3, _4, 0.f))));
  lib.Register ("EnableBackground",    make_invoker (&Viewport::EnableBackground));
  lib.Register ("DisableBackground",   make_invoker (&Viewport::DisableBackground));
  lib.Register ("set_Properties",      make_invoker (&Viewport::SetProperties));
  lib.Register ("get_Properties",      make_invoker (&Viewport::Properties));

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
  InvokerRegistry lib = environment.CreateLibrary (RENDER_SCREEN_LIBRARY);

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
  lib.Register ("get_Area",            make_invoker (&Screen::Area));
  lib.Register ("set_Area",            make_invoker (implicit_cast<void (Screen::*) (const Rect&)> (&Screen::SetArea)));

  lib.Register ("SetArea",             make_invoker (implicit_cast<void (Screen::*) (int, int, int, int)> (&Screen::SetArea)));
  lib.Register ("SetOrigin",           make_invoker (&Screen::SetOrigin));
  lib.Register ("SetSize",             make_invoker (&Screen::SetSize));
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

/*
    Регистрация библиотеки рендера
*/

void bind_screen_viewport_library (Environment& environment)
{
    //регистрация библиотек
  
  bind_rect_library     (environment);
  bind_viewport_library (environment);
  bind_screen_library   (environment);
}

}

}
