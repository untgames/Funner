#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/ref.h>

#include <common/var_registry.h>
#include <common/var_registry_container.h>
#include <common/strlib.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/mid_level/low_level_driver.h>

#include "shared.h"

using namespace common;
using namespace render::low_level;

namespace
{

/*
    Константы
*/

const char* CONFIGURATION_FILE_NAME   = "media/configuration.xml"; //имя файла конфигурации
const char* CONFIGURATION_BRANCH_NAME = "Demo"; //имя ветки реестра с настройками
const char* MID_LEVEL_RENDERER_NAME   = "MyRenderer"; //имя системы визуализации среднего уровня
const char* MATERIAL_LIB_FILE_NAME    = "media/materials.xmtl"; //имя файла с материалами

const size_t DEFAULT_WINDOW_WIDTH  = 400;             //начальная ширина окна
const size_t DEFAULT_WINDOW_HEIGHT = 300;             //начальная высота окна
const char*  DEFAULT_WINDOW_TITLE  = "Render2d test"; //заголовок окна

const size_t DEFAULT_FB_COLOR_BITS    = 24; //глубина буфера цвета
const size_t DEFAULT_FB_ALPHA_BITS    = 8;  //глубина альфа-буфера
const size_t DEFAULT_FB_DEPTH_BITS    = 24; //глубина z-buffer'а
const size_t DEFAULT_FB_STENCIL_BITS  = 8;  //глубина буфера трафарета
const size_t DEFAULT_FB_BUFFERS_COUNT = 2;  //количество буферов в цепочке обмена
const size_t DEFAULT_FB_FULL_SCREEN_STATE = 0; //fullscreen по умолчанию

const char* DEFAULT_DEVICE_INIT_STRING = ""; //строка инициализации устройства рендеринга

/*
    Утилиты
*/

template <class T> T get (VarRegistry& registry, const char* name, const T& default_value)
{
//  printf ("Read value of %s, branch name is %s\n", name, registry.BranchName ());

  if (!registry.HasVariable (name))
    return default_value;
    
//  printf ("Readed value of %s\n", name);

  try
  {
    return xtl::any_multicast<T> (registry.GetValue (name));
  }
  catch (...)
  {
    return default_value;
  }
}

stl::string get (VarRegistry& registry, const char* name, const char* default_value) //??????
{
  if (!registry.HasVariable (name))
    return default_value;

  return to_string (registry.GetValue (name));
}

void log_print (const char* message)
{
  printf ("%s\n", message);
}

}

/*
    Описание реализации TestApplication
*/

typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
typedef VarRegistryContainer<stl::string>           StringRegistry;
typedef xtl::com_ptr<input::low_level::IDevice>     InputDevicePtr;

struct TestApplication::Impl
{
  StringRegistry                string_registry;     //монтируемый реестр строк  
  VarRegistry                   config;              //реестр конфигурационных настроек
  stl::auto_ptr<syslib::Window> window;              //главное окно приложения
  SwapChainPtr                  swap_chain;          //цепочка обмена главного окна приложения
  DevicePtr                     device;              //устройство рендеринга главного окна приложения
  xtl::auto_connection          app_idle_connection; //соединение сигнала обработчика холостого хода приложения
  SceneRender                   render;              //рендер сцены
  InputDevicePtr                input_device;        //устройство ввода
  
  void OnClose ()
  {
    syslib::Application::Exit (0);
  }

  void OnRedraw ()
  {
    try
    {
      static clock_t last     = 0;  
      static size_t  last_fps = 0, frames_count = 0;
    
      if (clock () - last_fps > CLK_TCK)
      {
        printf ("FPS: %.2f\n", float (frames_count)/float (clock () - last_fps)*float (CLK_TCK));

        last_fps     = clock ();
        frames_count = 0;
      }

      render.Draw ();
      
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
  
  void OnResize ()
  {
    try
    {
      syslib::Rect            client_rect = window->ClientRect ();
      render::low_level::Rect viewport;

      viewport.x      = client_rect.left;
      viewport.y      = client_rect.top;
      viewport.width  = client_rect.right - client_rect.left;
      viewport.height = client_rect.bottom - client_rect.top;

      mid_level::LowLevelDriver::SetViewport (MID_LEVEL_RENDERER_NAME, viewport);

      window->Invalidate ();
    }
    catch (std::exception& exception)
    {
      printf ("Eexception at window resize: %s\n", exception.what ());
    }
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
      //монтирование реестра
      
    impl->string_registry.Mount (CONFIGURATION_BRANCH_NAME);
    
      //чтение настроек
      
    impl->config.Open (CONFIGURATION_BRANCH_NAME);

    load_xml_configuration (VarRegistry (""), CONFIGURATION_FILE_NAME);

      //создание окна
      
    impl->window = new syslib::Window (get (impl->config, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ? 
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, get (impl->config, "WindowWidth", DEFAULT_WINDOW_WIDTH),
        get (impl->config, "WindowHeight", DEFAULT_WINDOW_HEIGHT));
        
    impl->window->SetDebugLog (&log_print);

    impl->window->SetTitle (get (impl->config, "WindowTitle", DEFAULT_WINDOW_TITLE).c_str ());    

    impl->window->Show ();
    
      //регистрация обработчиков событий окна

    impl->window->RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnSize,  xtl::bind (&Impl::OnResize, &*impl));
    impl->window->RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, &*impl));    

      //создание цепочки обмена и устройства рендеринга

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = get (impl->config, "ColorBits", DEFAULT_FB_COLOR_BITS);
    desc.frame_buffer.alpha_bits   = get (impl->config, "AlphaBits", DEFAULT_FB_ALPHA_BITS);
    desc.frame_buffer.depth_bits   = get (impl->config, "DepthBits", DEFAULT_FB_DEPTH_BITS);
    desc.frame_buffer.stencil_bits = get (impl->config, "StencilBits", DEFAULT_FB_STENCIL_BITS);
    desc.buffers_count             = get (impl->config, "BuffersCount", DEFAULT_FB_BUFFERS_COUNT);
    desc.swap_method               = SwapMethod_Discard;
    desc.fullscreen                = get (impl->config, "FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) != 0;
    desc.window_handle             = impl->window->Handle ();
    
    render::low_level::DriverManager::CreateSwapChainAndDevice ("*", desc, get (impl->config, "DeviceInitString", DEFAULT_DEVICE_INIT_STRING).c_str (),
      impl->swap_chain, impl->device);
      
      //создание системы визуализации среднего уровня
      
    render::mid_level::LowLevelDriver::RegisterRenderer (MID_LEVEL_RENDERER_NAME, impl->device.get (), impl->swap_chain.get ());
    
      //инициализация рендера

    impl->render.SetLogHandler (&log_print);

    impl->render.SetRenderer (render::mid_level::LowLevelDriver::Name (), MID_LEVEL_RENDERER_NAME);
//    impl->render.SetRenderer ("Debug", "Renderer2d");

      //загрузка ресурсов

    impl->render.LoadResource (MATERIAL_LIB_FILE_NAME);
    
      //установка начальной области вывода
    
    impl->OnResize ();    

      //инициализация системы ввода

    input::low_level::WindowDriver::RegisterDevice (*(impl->window.get ()));

    impl->input_device = InputDevicePtr (input::low_level::DriverManager::CreateDevice ("*", "*"), false);
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

SceneRender& TestApplication::Render ()
{
  return impl->render;
}

input::low_level::IDevice& TestApplication::InputDevice ()
{
  return *(impl->input_device);
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
