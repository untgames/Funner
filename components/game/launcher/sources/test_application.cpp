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

const char* CONFIGURATION_FILE_NAME   = "data/configurations/configuration.xml"; //имя файла конфигурации
const char* CONFIGURATION_BRANCH_NAME = "Configuration"; //имя ветки реестра с настройками
const char* MID_LEVEL_RENDERER_NAME   = "MyRenderer"; //имя системы визуализации среднего уровня
const char* MATERIAL_LIB_FILE_NAME    = "data/materials/materials.xmtl"; //имя файла с материалами
const char* SOUND_DECL_LIB_FILE_NAME  = "data/sounds/gorilka.snddecl"; //имя файла с материалами

const char* TRANSLATION_MAP_FILE_NAME = "data/configurations/input/translation_table.keymap";

const char* DEFAULT_SOUND_DEVICE_MASK = "*"; //маска имени устройства воспроизведения

const size_t DEFAULT_WINDOW_WIDTH  = 100;             //начальная ширина окна
const size_t DEFAULT_WINDOW_HEIGHT = 100;             //начальная высота окна
const char*  DEFAULT_WINDOW_TITLE  = "Default title"; //заголовок окна

const size_t DEFAULT_FB_COLOR_BITS    = 24; //глубина буфера цвета
const size_t DEFAULT_FB_ALPHA_BITS    = 8;  //глубина альфа-буфера
const size_t DEFAULT_FB_DEPTH_BITS    = 24; //глубина z-buffer'а
const size_t DEFAULT_FB_STENCIL_BITS  = 8;  //глубина буфера трафарета
const size_t DEFAULT_FB_BUFFERS_COUNT = 2;  //количество буферов в цепочке обмена
const size_t DEFAULT_FB_FULL_SCREEN_STATE = 0; //fullscreen по умолчанию

static clock_t MOVE_PERIOD = CLOCKS_PER_SEC / 100;

const char* DEFAULT_DEVICE_INIT_STRING = ""; //строка инициализации устройства рендеринга

const char* APPLICATION_LIBRARY = "static.Application";

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

void do_file (const char* file_name, Shell& shell)
{
  shell.ExecuteFile (file_name, &log_print);
}

void idle (TestApplication& app, Shell& shell)
{
  try
  {
    static clock_t last_time = clock ();
    
    if (clock () - last_time > MOVE_PERIOD)
    {
      invoke<void> (*shell.Interpreter (), "idle", float (clock () - last_time) / CLOCKS_PER_SEC);

      last_time = clock ();
    }        

    app.PostRedraw ();
  }
  catch (std::exception& exception)
  {
    printf ("exception at idle: %s\n", exception.what ());
  }
}

/*
    Выполнение скрипта    
*/

void input_event_handler (const char* event, Shell& shell)
{
  shell.Execute (event, &log_print);
}

}

/*
    Описание реализации TestApplication
*/

typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
typedef VarRegistryContainer<stl::string>           StringRegistry;
typedef xtl::com_ptr<input::low_level::IDevice>     InputDevicePtr;
typedef xtl::shared_ptr<sound::SoundManager>        SoundManagerPtr;
typedef xtl::shared_ptr<sound::ScenePlayer>         ScenePlayerPtr;
typedef xtl::shared_ptr<Environment>                EnvironmentPtr;
typedef xtl::shared_ptr<Shell>                      ShellPtr;

struct TestApplication::Impl
{
  StringRegistry                string_registry;     //монтируемый реестр строк  
  VarRegistry                   config;              //реестр конфигурационных настроек
  stl::auto_ptr<syslib::Window> window;              //главное окно приложения
  SwapChainPtr                  swap_chain;          //цепочка обмена главного окна приложения
  DevicePtr                     device;              //устройство рендеринга главного окна приложения
  xtl::auto_connection          app_idle_connection; //соединение сигнала обработчика холостого хода приложения
  SceneRender                   render;              //рендер сцены
  render::RenderTarget          render_target;       //цель рендеринга  
  InputDevicePtr                input_device;        //устройство ввода
  SoundManagerPtr               sound_manager;       //менеджер звука
  ScenePlayerPtr                scene_player;        //проигрыватель звуков сцены
  EnvironmentPtr                environment;         //скриптовое окружение
  ShellPtr                      shell;               //скриптовая оболочка
  render::Viewport              viewport;            //вьюпорт
  Desktop                       desktop;    
  input::TranslationMap         translation_map;

        
  Impl ()
  {
      //монтирование реестра
      
    string_registry.Mount (CONFIGURATION_BRANCH_NAME);

      //чтение настроек
      
    config.Open (CONFIGURATION_BRANCH_NAME);
  }

  void OnClose ()
  {
    app_idle_connection.disconnect ();
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
  
  void OnResize ()
  {
    try
    {
      syslib::Rect client_rect = window->ClientRect ();

      render_target.SetRenderableArea (client_rect.left, client_rect.top, client_rect.right - client_rect.left,
                                       client_rect.bottom - client_rect.top);

      window->Invalidate ();            
    }
    catch (std::exception& exception)
    {
      printf ("Eexception at window resize: %s\n", exception.what ());
    }
  }

  void LoadConfiguration (const char* configuration_file_name)
  {
    load_xml_configuration (VarRegistry (""), configuration_file_name);
  }

  void SetCamera (Camera* camera)
  {
    viewport.SetCamera (camera);
  }

  void SetListener (Listener* listener)
  {
    scene_player->SetListener (listener);
  }
};

/*
    Конструктор / деструктор
*/

TestApplication::TestApplication (const char* start_script_name)
  : impl (new Impl)
{
  try
  {
      //инициализация звука

    impl->scene_player  = ScenePlayerPtr  (new sound::ScenePlayer ());

      //инициализация LUA

    impl->environment = EnvironmentPtr (new Environment);

    impl->shell = ShellPtr (new Shell ("lua", impl->environment));

    bind_math_library (*impl->environment);
    bind_scene_graph_library (*impl->environment);

    InvokerRegistry& application_library = impl->environment->CreateLibrary (APPLICATION_LIBRARY);

    application_library.Register ("LoadConfiguration", make_invoker<void (const char*)> (xtl::bind (&TestApplication::Impl::LoadConfiguration, impl.get (), _1)));
    application_library.Register ("Exit", make_invoker<void ()> (xtl::bind (&TestApplication::Impl::OnClose, impl.get ())));
    application_library.Register ("SetCamera", make_invoker<void (Camera*)> (xtl::bind (&TestApplication::Impl::SetCamera, impl.get (), _1)));
    application_library.Register ("SetListener", make_invoker<void (Listener*)> (xtl::bind (&TestApplication::Impl::SetListener, impl.get (), _1)));

    InvokerRegistry& lib = impl->environment->Library ("global");

    lib.Register ("dofile", make_invoker<void (const char*)> (xtl::bind (&do_file, _1, xtl::ref (*impl->shell))));

    impl->shell->ExecuteFile (start_script_name, &log_print);

      //инициализация звука

    impl->sound_manager = SoundManagerPtr (new sound::SoundManager ("OpenAL", get (impl->config, "SoundDeviceMask", DEFAULT_SOUND_DEVICE_MASK).c_str ()));

    impl->sound_manager->LoadSoundLibrary (SOUND_DECL_LIB_FILE_NAME);

    impl->scene_player->SetManager (impl->sound_manager.get ());

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

    impl->render_target = impl->render.CreateRenderTarget ("default", "default");

      //загрузка ресурсов

    impl->render.LoadResource (MATERIAL_LIB_FILE_NAME);
    
      //установка начальной области вывода
    
    impl->OnResize ();    

      //инициализация системы ввода

    input::low_level::WindowDriver::RegisterDevice (*(impl->window.get ()));

    impl->input_device = InputDevicePtr (input::low_level::DriverManager::CreateDevice ("*", "*"), false);
    
      //создание областей вывода
      
    impl->viewport.SetName       ("Viewport1");
    impl->viewport.SetRenderPath ("Render2d");
    
    impl->viewport.SetArea (0, 0, 100, 100);

    impl->desktop.SetBackgroundColor (math::vec4f (1, 1, 1, 1));    
    impl->desktop.Attach (impl->viewport);

    impl->render_target.SetDesktop (&impl->desktop);        

      //инициализация системы ввода

    impl->translation_map.Load (TRANSLATION_MAP_FILE_NAME);

    input::TranslationMap::EventHandler event_handler = xtl::bind (&input_event_handler, _1, xtl::ref (*impl->shell));

    impl->input_device->SetEventHandler (xtl::bind (&input::TranslationMap::ProcessEvent, &impl->translation_map, _1, event_handler));
    
      //установка idle-функции

    impl->app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (*this), xtl::ref (*impl->shell)));
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
