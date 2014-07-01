#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <xtl/any.h>

#include <common/strlib.h>

#include <syslib/application.h>

#include <render/scene_render_client.h>
#include <render/scene_render_server.h>

#include "shared.h"

using namespace common;
using namespace render;

namespace
{

/*
    Константы
*/

const char* CONFIGURATION_FILE_NAME   = "media/config.xml"; //имя файла конфигурации
const char* SERVER_NAME               = "MyServer";         //имя сервера рендеринга
const char* MATERIAL_LIB_FILE_NAME    = "media/materials.xmtl"; //имя файла с материалами

const size_t DEFAULT_WINDOW_WIDTH  = 400;             //начальная ширина окна
const size_t DEFAULT_WINDOW_HEIGHT = 300;             //начальная высота окна
const char*  DEFAULT_WINDOW_TITLE  = "Render2d test"; //заголовок окна

const size_t DEFAULT_FB_COLOR_BITS         = 24; //глубина буфера цвета
const size_t DEFAULT_FB_ALPHA_BITS         = 8;  //глубина альфа-буфера
const size_t DEFAULT_FB_DEPTH_BITS         = 24; //глубина z-buffer'а
const size_t DEFAULT_FB_STENCIL_BITS       = 8;  //глубина буфера трафарета
const bool   DEFAULT_FB_FULL_SCREEN_STATE  = 0;  //состояние full screen

/*
    Утилиты
*/

template <class T> T get (ParseNode& node, const char* name, const T& default_value)
{
  return common::get<T> (node, name, default_value);
}

void log_print (const char* message)
{
  printf ("%s\n", message);
}

}

/*
    Описание реализации TestApplication
*/

struct TestApplication::Impl
{
  stl::auto_ptr<syslib::Window>  window;              //главное окно приложения
  xtl::auto_connection           app_idle_connection; //соединение сигнала обработчика холостого хода приложения
  scene::server::Server          render_server;       //сервер рендера сцены
  scene::client::Client          render_client;       //клиент рендера сцены
  scene::client::RenderTarget    render_target;       //цель рендеринга

  Impl ()
    : render_server (SERVER_NAME)
    , render_client (SERVER_NAME)
  {
  }
  
  void OnClose ()
  {
    syslib::Application::Exit (0);

    app_idle_connection.disconnect ();
  }

  void OnRedraw ()
  {
    try
    {
      static size_t  last_fps = 0, frames_count = 0;

      if (common::milliseconds () - last_fps > 1000)
      {
        printf ("FPS: %.2f\n", float (frames_count) / float (milliseconds () - last_fps) * 1000.f);

        last_fps     = common::milliseconds ();
        frames_count = 0;
      }

      render_target.Update ();

      frames_count++;
    }
    catch (std::exception& e)
    {
      printf (format ("Exception at window redraw: %s\n", e.what ()).c_str ());
    }
    catch (...)
    {
      printf ("Exception at window redraw\n");
    }
  }
  
  void ChangeWindowStyle ()
  {
    printf ("Change window style\n");

    if (window)
      window->SetStyle ((syslib::WindowStyle)((window->Style () + 1) % 2));    
  }
};

/*
    Конструктор / деструктор
*/

TestApplication::TestApplication ()
  : impl (new Impl)
{
  try
  {
    common::Parser cfg_parser (CONFIGURATION_FILE_NAME);
    common::ParseNode cfg_root = cfg_parser.Root ().First ("Configuration");    
    
      //создание окна

    impl->window = new syslib::Window (get (cfg_root, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ?
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, get (cfg_root, "WindowWidth", DEFAULT_WINDOW_WIDTH),
        get (cfg_root, "WindowHeight", DEFAULT_WINDOW_HEIGHT));

    impl->window->SetDebugLog (&log_print);

    impl->window->SetTitle (get (cfg_root, "WindowTitle", DEFAULT_WINDOW_TITLE));

    impl->window->Show ();

      //регистрация обработчиков событий окна

    impl->window->RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnLeftButtonDown, xtl::bind (&Impl::ChangeWindowStyle, &*impl));    

      //инициализация системы рендеринга

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", get<int> (cfg_root, "ColorBits", DEFAULT_FB_COLOR_BITS));
    window_properties.SetProperty ("DepthBits", get<int> (cfg_root, "DepthBits", DEFAULT_FB_DEPTH_BITS));
    window_properties.SetProperty ("AlphaBits", get<int> (cfg_root, "AlphaBits", DEFAULT_FB_ALPHA_BITS));
    window_properties.SetProperty ("AlphaBits", get<int> (cfg_root, "StencilBits", DEFAULT_FB_STENCIL_BITS));

    impl->render_server.AttachWindow ("my_window", *impl->window, window_properties);

      //инициализация рендера

    impl->render_target = impl->render_client.CreateRenderTarget ("my_window");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("TestApplication::TestApplication");
    throw;
  }
}

TestApplication::~TestApplication ()
{
}

/*
    Получение объектов приложения
*/

scene::client::Client& TestApplication::Render ()
{
  return impl->render_client;
}

/*
   Загрузка ресурсов
*/

void TestApplication::LoadResources ()
{
  impl->render_client.LoadResource (MATERIAL_LIB_FILE_NAME);
}

/*
   Получение окна приложения
*/

syslib::Window& TestApplication::Window ()
{
  return *impl->window;
}

/*
    Получение цели рендеринга
*/

scene::client::RenderTarget& TestApplication::RenderTarget ()
{
  return impl->render_target;
}

/*
    Запуск приложения
*/

int TestApplication::Run ()
{
  syslib::Application::Run ();

  return syslib::Application::GetExitCode ();
}

/*
    Перерисовка
*/

void TestApplication::PostRedraw ()
{
  impl->window->Invalidate ();
}

/*
    Установка idle-функции
*/

void TestApplication::SetIdleHandler (const IdleFunction& idle)
{
  impl->app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (idle, xtl::ref (*this)));
}
