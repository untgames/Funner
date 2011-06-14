#include "shared.h"

#include <syslib/application.h>
#include <syslib/window.h>

using namespace script;
using namespace syslib;

namespace
{

/*
    Константы
*/

const char* APPLICATION_LIBRARY       = "System.Application";
const char* APPLICATION_EVENT_LIBRARY = "System.ApplicationEvent";
const char* WINDOW_LIBRARY            = "System.Window";
const char* COMPONENT_NAME            = "script.binds.System";
const char* BINDER_NAME               = "System";

/*
    Утилиты
*/

void bind_application_events_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (APPLICATION_EVENT_LIBRARY);

  lib.Register ("get_OnExit", make_const (ApplicationEvent_OnExit));
  lib.Register ("get_OnIdle", make_const (ApplicationEvent_OnIdle));
}

void bind_application_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (APPLICATION_LIBRARY);

    //регистрация операций

  lib.Register ("Exit",                 make_invoker (&syslib::Application::Exit));
  lib.Register ("Sleep",                make_invoker (&syslib::Application::Sleep));
  lib.Register ("OpenUrl",              make_invoker (&syslib::Application::OpenUrl));
  lib.Register ("CreateEventHandler",   make_callback_invoker<syslib::Application::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (&syslib::Application::RegisterEventHandler));
}

void set_window_x (Window& window, int x)
{
  window.SetPosition (x, window.Position ().y);
}

int get_window_x (Window& window)
{
  return window.Position ().x;
}

void set_window_y (Window& window, int y)
{
  window.SetPosition (window.Position ().x, y);
}

int get_window_y (Window& window)
{
  return window.Position ().y;
}

math::vec2f get_window_position (Window& window)
{
  return math::vec2f ((float)window.Position ().x, (float)window.Position ().x);
}

void set_window_position (Window& window, const math::vec2f& pos)
{
  window.SetPosition ((int)pos.x, (int)pos.y);
}

math::vec2f get_window_size (Window& window)
{
  return math::vec2f ((float)window.Width (), (float)window.Height ());
}

void set_window_size (Window& window, const math::vec2f& size)
{
  window.SetSize ((size_t)size.x, (size_t)size.y);
}

math::vec2f get_window_client_size (Window& window)
{
  return math::vec2f ((float)window.ClientWidth (), (float)window.ClientHeight ());
}

void set_window_client_size (Window& window, const math::vec2f& size)
{
  window.SetClientSize ((size_t)size.x, (size_t)size.y);
}

void bind_window_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (WINDOW_LIBRARY);

    //регистрация операций

  lib.Register ("set_Title",        make_invoker ((void (syslib::Window::*)(const char*))&syslib::Window::SetTitle));
  lib.Register ("get_Title",        make_invoker (&syslib::Window::Title));
  lib.Register ("get_X",            make_invoker (&get_window_x));  
  lib.Register ("set_X",            make_invoker (&set_window_x));
  lib.Register ("get_Y",            make_invoker (&get_window_y));  
  lib.Register ("set_Y",            make_invoker (&set_window_y));
  lib.Register ("get_Position",     make_invoker (&get_window_position));
  lib.Register ("get_Position",     make_invoker (&get_window_position));
  lib.Register ("SetPosition",      make_invoker ((void (syslib::Window::*)(size_t, size_t))&syslib::Window::SetPosition));
  lib.Register ("set_Position",     make_invoker (&set_window_position));
  lib.Register ("set_Position",     make_invoker (&set_window_position));    
  lib.Register ("set_Width",        make_invoker (&syslib::Window::SetWidth));
  lib.Register ("set_Height",       make_invoker (&syslib::Window::SetHeight));
  lib.Register ("get_Width",        make_invoker (&syslib::Window::Width));
  lib.Register ("get_Height",       make_invoker (&syslib::Window::Height));
  lib.Register ("get_Size",         make_invoker (&get_window_size));  
  lib.Register ("set_Size",         make_invoker (&set_window_size));    
  lib.Register ("SetSize",          make_invoker (&syslib::Window::SetSize));      
  lib.Register ("set_ClientWidth",  make_invoker (&syslib::Window::SetClientWidth));
  lib.Register ("set_ClientHeight", make_invoker (&syslib::Window::SetClientHeight));  
  lib.Register ("get_ClientWidth",  make_invoker (&syslib::Window::ClientWidth));
  lib.Register ("get_ClientHeight", make_invoker (&syslib::Window::ClientHeight));
  lib.Register ("get_ClientSize",   make_invoker (&get_window_client_size));  
  lib.Register ("set_ClientSize",   make_invoker (&set_window_client_size));  
  lib.Register ("SetClientSize",    make_invoker (&syslib::Window::SetClientSize));    
  lib.Register ("get_Visible",      make_invoker (&syslib::Window::IsVisible));
  lib.Register ("set_Visible",      make_invoker (&syslib::Window::SetVisible));
  lib.Register ("get_Active",       make_invoker (&syslib::Window::IsActive));  
  lib.Register ("set_Active",       make_invoker (&syslib::Window::SetActive));    
  lib.Register ("get_Focus",        make_invoker (&syslib::Window::HasFocus));
  lib.Register ("set_Focus",        make_invoker (&syslib::Window::SetFocus));  
  lib.Register ("Maximize",         make_invoker (&syslib::Window::Maximize));
  lib.Register ("Minimize",         make_invoker (&syslib::Window::Minimize));
  lib.Register ("Close",            make_invoker (&syslib::Window::Close));
  lib.Register ("CancelClose",      make_invoker (&syslib::Window::CancelClose));
  lib.Register ("ForceClose",       make_invoker (&syslib::Window::ForceClose));
  
  environment.RegisterType<Window> (WINDOW_LIBRARY);
}

void bind_syslib_library (Environment& environment)
{
  bind_application_events_library (environment);
  bind_application_library        (environment);
  bind_window_library             (environment);
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
      bind_syslib_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SystemScriptBind (COMPONENT_NAME);

}

}
