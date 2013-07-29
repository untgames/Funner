#include "shared.h"

#include <syslib/application.h>
#include <syslib/cookie.h>
#include <syslib/screen.h>
#include <syslib/screen_keyboard.h>
#include <syslib/web_view.h>
#include <syslib/window.h>

using namespace script;
using namespace syslib;

namespace
{

/*
    Константы
*/

const char* APPLICATION_BACKGROUND_MODE_LIBRARY = "System.ApplicationBackgroundMode";
const char* APPLICATION_LIBRARY                 = "System.Application";
const char* APPLICATION_EVENT_LIBRARY           = "System.ApplicationEvent";
const char* COOKIE_MANAGER_LIBRARY              = "System.CookieManager";
const char* SCREEN_KEYBOARD_LIBRARY             = "System.ScreenKeyboard";
const char* SCREEN_KEYBOARD_TYPE_LIBRARY        = "System.ScreenKeyboardType";
const char* SCREEN_LIBRARY                      = "System.Screen";
const char* SCREEN_MANAGER_LIBRARY              = "System.ScreenManager";
const char* WEB_VIEW_LIBRARY                    = "System.WebView";
const char* WEB_VIEW_EVENT_LIBRARY              = "System.WebViewEvent";
const char* WINDOW_STYLE_LIBRARY                = "System.WindowStyle";
const char* WINDOW_LIBRARY                      = "System.Window";
const char* COMPONENT_NAME                      = "script.binds.System";
const char* BINDER_NAME                         = "System";

/*
    Утилиты
*/

void bind_application_background_mode_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (APPLICATION_BACKGROUND_MODE_LIBRARY);

  lib.Register ("get_Active",  make_const (ApplicationBackgroundMode_Active));
  lib.Register ("get_Suspend", make_const (ApplicationBackgroundMode_Suspend));
}

void bind_application_events_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (APPLICATION_EVENT_LIBRARY);

  lib.Register ("get_OnExit",   make_const (ApplicationEvent_OnExit));
  lib.Register ("get_OnIdle",   make_const (ApplicationEvent_OnIdle));
  lib.Register ("get_OnPause",  make_const (ApplicationEvent_OnPause));
  lib.Register ("get_OnResume", make_const (ApplicationEvent_OnResume));
}

void bind_window_styles_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (WINDOW_STYLE_LIBRARY);

  lib.Register ("get_Overlapped", make_const (WindowStyle_Overlapped));
  lib.Register ("get_PopUp",      make_const (WindowStyle_PopUp));
}

void bind_application_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (APPLICATION_LIBRARY);

    //регистрация операций

  lib.Register ("get_BackgroundMode",          make_invoker (&syslib::Application::BackgroundMode));
  lib.Register ("get_ScreenSaverState",        make_invoker (&syslib::Application::ScreenSaverState));
  lib.Register ("set_BackgroundMode",          make_invoker (&syslib::Application::SetBackgroundMode));
  lib.Register ("set_ScreenSaverState",        make_invoker (&syslib::Application::SetScreenSaverState));
  lib.Register ("Exit",                        make_invoker (&syslib::Application::Exit));
  lib.Register ("Sleep",                       make_invoker (&syslib::Application::Sleep));
  lib.Register ("OpenUrl",                     make_invoker (&syslib::Application::OpenUrl));
  lib.Register ("GetEnvironmentVariable",      make_invoker (&syslib::Application::GetEnvironmentVariable));
  lib.Register ("CreateEventHandler",          make_callback_invoker<syslib::Application::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler",        make_invoker (&syslib::Application::RegisterEventHandler));
  lib.Register ("PostNotification",            make_invoker (&syslib::Application::PostNotification));
  lib.Register ("CreateNotificationHandler",   make_callback_invoker<syslib::Application::NotificationHandler::signature_type> ());
  lib.Register ("RegisterNotificationHandler", make_invoker (&syslib::Application::RegisterNotificationHandler));
  lib.Register ("get_SystemProperties",        make_invoker (&syslib::Application::SystemProperties));
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

  lib.Register ("set_Title",            make_invoker ((void (syslib::Window::*)(const char*))&syslib::Window::SetTitle));
  lib.Register ("get_Title",            make_invoker (&syslib::Window::Title));
  lib.Register ("get_X",                make_invoker (&get_window_x));
  lib.Register ("set_X",                make_invoker (&set_window_x));
  lib.Register ("get_Y",                make_invoker (&get_window_y));
  lib.Register ("set_Y",                make_invoker (&set_window_y));
  lib.Register ("get_Position",         make_invoker (&get_window_position));
  lib.Register ("get_Position",         make_invoker (&get_window_position));
  lib.Register ("SetPosition",          make_invoker ((void (syslib::Window::*)(size_t, size_t))&syslib::Window::SetPosition));
  lib.Register ("set_Position",         make_invoker (&set_window_position));
  lib.Register ("set_Position",         make_invoker (&set_window_position));
  lib.Register ("set_Width",            make_invoker (&syslib::Window::SetWidth));
  lib.Register ("set_Height",           make_invoker (&syslib::Window::SetHeight));
  lib.Register ("get_Width",            make_invoker (&syslib::Window::Width));
  lib.Register ("get_Height",           make_invoker (&syslib::Window::Height));
  lib.Register ("get_Size",             make_invoker (&get_window_size));
  lib.Register ("set_Size",             make_invoker (&set_window_size));
  lib.Register ("SetSize",              make_invoker (&syslib::Window::SetSize));
  lib.Register ("set_ClientWidth",      make_invoker (&syslib::Window::SetClientWidth));
  lib.Register ("set_ClientHeight",     make_invoker (&syslib::Window::SetClientHeight));
  lib.Register ("get_ClientWidth",      make_invoker (&syslib::Window::ClientWidth));
  lib.Register ("get_ClientHeight",     make_invoker (&syslib::Window::ClientHeight));
  lib.Register ("get_ClientSize",       make_invoker (&get_window_client_size));
  lib.Register ("set_ClientSize",       make_invoker (&set_window_client_size));
  lib.Register ("SetClientSize",        make_invoker (&syslib::Window::SetClientSize));
  lib.Register ("get_Visible",          make_invoker (&syslib::Window::IsVisible));
  lib.Register ("set_Visible",          make_invoker (&syslib::Window::SetVisible));
  lib.Register ("get_Active",           make_invoker (&syslib::Window::IsActive));
  lib.Register ("set_Active",           make_invoker (&syslib::Window::SetActive));
  lib.Register ("get_Focus",            make_invoker (&syslib::Window::HasFocus));
  lib.Register ("set_Focus",            make_invoker (&syslib::Window::SetFocus));
  lib.Register ("get_Style",            make_invoker (&syslib::Window::Style));
  lib.Register ("set_Style",            make_invoker (&syslib::Window::SetStyle));
  lib.Register ("Maximize",             make_invoker (&syslib::Window::Maximize));
  lib.Register ("Minimize",             make_invoker (&syslib::Window::Minimize));
  lib.Register ("Close",                make_invoker (&syslib::Window::Close));
  lib.Register ("CancelClose",          make_invoker (&syslib::Window::CancelClose));
  lib.Register ("ForceClose",           make_invoker (&syslib::Window::ForceClose));
  lib.Register ("get_ContainingScreen", make_invoker (xtl::implicit_cast<Screen (syslib::Window::*)() const> (&syslib::Window::ContainingScreen)));
  
  environment.RegisterType<Window> (WINDOW_LIBRARY);
}

void bind_web_view_events_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (WEB_VIEW_EVENT_LIBRARY);

  lib.Register ("get_OnLoadStart",  make_const (WebViewEvent_OnLoadStart));
  lib.Register ("get_OnLoadFinish", make_const (WebViewEvent_OnLoadFinish));
  lib.Register ("get_OnLoadFail",   make_const (WebViewEvent_OnLoadFail));
  lib.Register ("get_OnClose",      make_const (WebViewEvent_OnClose));
  lib.Register ("get_OnDestroy",    make_const (WebViewEvent_OnDestroy));
}

xtl::connection register_web_view_event_handler (WebView& web_view, WebViewEvent event, const xtl::function<void (WebViewEvent)>& handler)
{
  return web_view.RegisterEventHandler (event, xtl::bind (handler, _2));
}

xtl::connection register_web_view_filter (WebView& web_view, const xtl::function<bool (const char*)>& filter)
{
  return web_view.RegisterFilter (xtl::bind (filter, _2));
}

xtl::trackable_ptr<Window> get_web_view_window (WebView& web_view)
{
  return &web_view.Window ();
}

xtl::shared_ptr<WebView> create_web_view ()
{
  return xtl::shared_ptr<WebView> (new WebView);
}

void bind_web_view_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (WEB_VIEW_LIBRARY);

    //регистрация операций

  lib.Register ("Create", make_invoker (&create_web_view));

  lib.Register ("get_Window",           make_invoker (&get_web_view_window));
  lib.Register ("get_IsLoading",        make_invoker (&WebView::IsLoading));
  lib.Register ("get_Request",          make_invoker (&WebView::Request));
  lib.Register ("get_Status",           make_invoker (&WebView::Status));
  lib.Register ("get_CanGoBack",        make_invoker (&WebView::CanGoBack));
  lib.Register ("get_CanGoForward",     make_invoker (&WebView::CanGoForward));
  lib.Register ("LoadRequest",          make_invoker (&WebView::LoadRequest));
  lib.Register ("LoadData",             make_invoker (&WebView::LoadData));
  lib.Register ("Reload",               make_invoker (&WebView::Reload));
  lib.Register ("StopLoading",          make_invoker (&WebView::StopLoading));
  lib.Register ("GoBack",               make_invoker (&WebView::GoBack));
  lib.Register ("GoForward",            make_invoker (&WebView::GoForward));
  lib.Register ("CreateFilter",         make_callback_invoker<bool (const char*)> ());
  lib.Register ("RegisterFilter",       make_invoker (&register_web_view_filter));
  lib.Register ("CreateEventHandler",   make_callback_invoker<void (WebViewEvent)> ());
  lib.Register ("RegisterEventHandler", make_invoker (&register_web_view_event_handler));

  environment.RegisterType<WebView> (WEB_VIEW_LIBRARY);
}

void bind_screen_keyboard_types_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCREEN_KEYBOARD_TYPE_LIBRARY);

  lib.Register ("get_Ascii",                make_const (ScreenKeyboardType_Ascii));
  lib.Register ("get_AsciiAutocapitalized", make_const (ScreenKeyboardType_AsciiAutocapitalized));
  lib.Register ("get_Number",               make_const (ScreenKeyboardType_Number));
  lib.Register ("get_NumberPunctuation",    make_const (ScreenKeyboardType_NumberPunctuation));
  lib.Register ("get_PlatformSpecific",     make_const (ScreenKeyboardType_PlatformSpecific));
}

xtl::shared_ptr<ScreenKeyboard> create_screen_keyboard (syslib::Window& window, ScreenKeyboardType type, const char* platform_specific)
{
  return xtl::shared_ptr<ScreenKeyboard> (new ScreenKeyboard (window, type, platform_specific));
}

void bind_screen_keyboard_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCREEN_KEYBOARD_LIBRARY);

  lib.Register ("Create", make_invoker (
      make_invoker (xtl::implicit_cast<xtl::shared_ptr<ScreenKeyboard> (*) (syslib::Window&, ScreenKeyboardType, const char*)> (&create_screen_keyboard)),
      make_invoker<xtl::shared_ptr<ScreenKeyboard> (syslib::Window&, ScreenKeyboardType)> (xtl::bind (&create_screen_keyboard, _1, _2, ""))
  ));

  lib.Register ("get_IsShown", make_invoker (&ScreenKeyboard::IsShown));
  lib.Register ("Show",        make_invoker (&ScreenKeyboard::Show));
  lib.Register ("Hide",        make_invoker (&ScreenKeyboard::Hide));
  lib.Register ("IsSupported", make_invoker (
      make_invoker (xtl::implicit_cast<bool (*)(ScreenKeyboardType)> (&ScreenKeyboard::IsSupported)),
      make_invoker (xtl::implicit_cast<bool (*)(const char*)> (&ScreenKeyboard::IsSupported))
  ));

  environment.RegisterType<ScreenKeyboard> (SCREEN_KEYBOARD_LIBRARY);
}

size_t get_screen_default_width (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;
  
  screen.GetDefaultMode (desc);

  return desc.width;
}

size_t get_screen_default_height (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;
  
  screen.GetDefaultMode (desc);

  return desc.height;
}

size_t get_screen_default_color_bits (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;
  
  screen.GetDefaultMode (desc);

  return desc.color_bits;
}

size_t get_screen_default_refresh_rate (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;
  
  screen.GetDefaultMode (desc);

  return desc.refresh_rate;
}

size_t get_screen_default_xdpi (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;

  screen.GetDefaultMode (desc);

  return desc.xdpi;
}

size_t get_screen_default_ydpi (const syslib::Screen& screen)
{
  syslib::ScreenModeDesc desc;

  screen.GetDefaultMode (desc);

  return desc.ydpi;
}

void bind_screen_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCREEN_LIBRARY);
  
  lib.Register ("get_Name",               make_invoker (&syslib::Screen::Name));
  lib.Register ("get_CurrentWidth",       make_invoker (&syslib::Screen::Width));
  lib.Register ("get_CurrentHeight",      make_invoker (&syslib::Screen::Height));
  lib.Register ("get_CurrentColorBits",   make_invoker (&syslib::Screen::ColorBits));
  lib.Register ("get_CurrentRefreshRate", make_invoker (&syslib::Screen::RefreshRate));
  lib.Register ("get_CurrentXdpi",        make_invoker (&syslib::Screen::Xdpi));
  lib.Register ("get_CurrentYdpi",        make_invoker (&syslib::Screen::Ydpi));
  lib.Register ("get_DefaultWidth",       make_invoker (&get_screen_default_width));
  lib.Register ("get_DefaultHeight",      make_invoker (&get_screen_default_height));
  lib.Register ("get_DefaultColorBits",   make_invoker (&get_screen_default_color_bits));
  lib.Register ("get_DefaultRefreshRate", make_invoker (&get_screen_default_refresh_rate));
  lib.Register ("get_DefaultXdpi",        make_invoker (&get_screen_default_xdpi));
  lib.Register ("get_DefaultYdpi",        make_invoker (&get_screen_default_ydpi));

  environment.RegisterType<syslib::Screen> (SCREEN_LIBRARY);
}

void bind_screen_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCREEN_MANAGER_LIBRARY);
  
  lib.Register ("get_ScreensCount", make_invoker (&syslib::ScreenManager::ScreensCount));
  lib.Register ("Screen",           make_invoker (&syslib::ScreenManager::Screen));
}

void bind_cookie_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (COOKIE_MANAGER_LIBRARY);

  lib.Register ("get_AcceptCookie", make_invoker (&syslib::CookieManager::AcceptCookie));
  lib.Register ("set_AcceptCookie", make_invoker (&syslib::CookieManager::SetAcceptCookie));
  lib.Register ("DeleteCookies",    make_invoker (&syslib::CookieManager::DeleteCookies));
  lib.Register ("DeleteAllCookies", make_invoker (&syslib::CookieManager::DeleteAllCookies));
}

void bind_syslib_library (Environment& environment)
{
  bind_application_background_mode_library (environment);
  bind_application_events_library          (environment);
  bind_application_library                 (environment);
  bind_cookie_manager_library              (environment);
  bind_window_styles_library               (environment);
  bind_window_library                      (environment);
  bind_web_view_events_library             (environment);
  bind_web_view_library                    (environment);
  bind_screen_keyboard_types_library       (environment);
  bind_screen_keyboard_library             (environment);
  bind_screen_library                      (environment);
  bind_screen_manager_library              (environment);
}

}

namespace components
{

namespace system_script_bind
{

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

}
