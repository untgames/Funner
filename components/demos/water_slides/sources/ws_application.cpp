#include "shared.h"

using namespace render::low_level;
using namespace common;

namespace
{

/*
    Константы (вынести в конфигурацию)!!!
*/

const char* CONFIG_FILE_NAME = "media/configuration.xml"; //имя файла конфигурации

const char* DEFAULT_LOG_FILE_NAME = "water-slides.log"; //имя файла протокола

const size_t DEFAULT_WINDOW_WIDTH  = 400;                         //начальная ширина окна
const size_t DEFAULT_WINDOW_HEIGHT = 300;                         //начальная высота окна
const char*  DEFAULT_WINDOW_TITLE  = "Water slides demo ($Id$)"; //заголовок окна

const char*  WATER_DROP_SOUND_NAME   = "media/drop.snddecl";      //имя файла декларации с звуком капли

const size_t DEFAULT_FB_COLOR_BITS    = 24; //глубина буфера цвета
const size_t DEFAULT_FB_ALPHA_BITS    = 8;  //глубина альфа-буфера
const size_t DEFAULT_FB_DEPTH_BITS    = 24; //глубина z-buffer'а
const size_t DEFAULT_FB_STENCIL_BITS  = 8;  //глубина буфера трафарета
const size_t DEFAULT_FB_BUFFERS_COUNT = 2;  //количество буферов в цепочке обмена
const size_t DEFAULT_FB_FULL_SCREEN_STATE = 0; //fullscreen по умолчанию

const char* DEFAULT_DEVICE_INIT_STRING = ""; //строка инициализации устройства рендеринга

}

/*
    Описание реализации приложения
*/

struct MyApplication::Impl
{
  public:
      //конструктор
    Impl () :
      configuration (CONFIG_FILE_NAME),
      log_file (configuration.GetString ("LogFileName", DEFAULT_LOG_FILE_NAME)),
      log_stream (xtl::bind (&Impl::LogWriteBuffer, this, _1, _2)),
      window (configuration.GetInteger ("FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) ? 
        syslib::WindowStyle_PopUp : syslib::WindowStyle_Overlapped, configuration.GetInteger ("WindowWidth", DEFAULT_WINDOW_WIDTH), configuration.GetInteger ("WindowHeight", DEFAULT_WINDOW_HEIGHT)),
      sound_manager (0),
      scene_player (0)
    {
      try
      {        
        sound_manager = new sound::SoundManager ("*", "*");

        LogMessage ("Loading sound declarations...");
        sound_manager->LoadSoundLibrary (WATER_DROP_SOUND_NAME);
      }
      catch (...)
      {
        delete sound_manager;
        sound_manager = 0;        

        LogMessage ("Can't create sound manager, exception caught");
      }      

      if (sound_manager)
      {
        try
        {
          scene_player = new sound::ScenePlayer;
          
          scene_player->SetManager (sound_manager);
        }
        catch (...)
        {
          delete sound_manager;
          sound_manager = 0;
          LogMessage ("Can't create scene player, exception caught");
        }
      }

      LogMessage ("Create window...");
      
        //установка пользовательской функции отладочного протоколирования

      window.SetDebugLog (xtl::bind (&Impl::LogMessage, this, _1));
      
        //установка заголовка окна
      
      window.SetTitle (configuration.GetString ("WindowTitle", DEFAULT_WINDOW_TITLE));
      
        //показ окна

      window.Show ();
      
      LogMessage ("Initializing rendering device...");

        //создание цепочки обмена и устройства рендеринга

      SwapChainDesc desc;

      memset (&desc, 0, sizeof (desc));

      desc.frame_buffer.color_bits   = configuration.GetInteger ("ColorBits", DEFAULT_FB_COLOR_BITS);
      desc.frame_buffer.alpha_bits   = configuration.GetInteger ("AlphaBits", DEFAULT_FB_ALPHA_BITS);
      desc.frame_buffer.depth_bits   = configuration.GetInteger ("DepthBits", DEFAULT_FB_DEPTH_BITS);
      desc.frame_buffer.stencil_bits = configuration.GetInteger ("StencilBits", DEFAULT_FB_STENCIL_BITS);
      desc.buffers_count             = configuration.GetInteger ("BuffersCount", DEFAULT_FB_BUFFERS_COUNT);
      desc.swap_method               = SwapMethod_Discard;
      desc.fullscreen                = configuration.GetInteger ("FullScreen", DEFAULT_FB_FULL_SCREEN_STATE) != 0;
      desc.window_handle             = window.Handle ();
      
      render::low_level::DriverManager::CreateSwapChainAndDevice ("*", "*", desc, configuration.GetString ("DeviceInitString", DEFAULT_DEVICE_INIT_STRING),
        swap_chain, device);
      
        //оповещение об изменении размеров
      
      OnResize ();

        //регистрация обработчиков событий окна

      window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Impl::OnResize, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, this));
      
      for (int i=syslib::WindowEvent_OnLeftButtonDown; i<=syslib::WindowEvent_OnMiddleButtonDoubleClick; i++)
        window.RegisterEventHandler ((syslib::WindowEvent)i, xtl::bind (&Impl::OnMouse, this, _2, _3));
        
      window.RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Impl::OnMouse, this, _2, _3));        

        //регистрация обработчиков событий приложения
        
      app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&Impl::OnIdle, this));      
    }
    
      //деструктор
    ~Impl ()
    {
      try
      {
        delete sound_manager;
        delete scene_player;

        LogMessage ("Close application");        
        
        SetView (GameView ());                

        LogMessage ("Destroy rendering device...");
        
        device = 0;
        swap_chain = 0;
        
        LogMessage ("Close window...");
        
        window.Close ();
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
    
      //протолирование
    void LogMessage (const char* message)
    {
      if (!message)
        return;

          //формирование префикса протоколирования

      time_t ltime;

      time (&ltime);

      stl::string prefix = common::format ("[%s", ctime (&ltime));
      
      prefix.pop_back ();

      prefix += common::format (" : %03u] ", MyApplication::Milliseconds () % 1000);

        //формирование набора строк для вывода в протокол

      stl::vector<stl::string> lines = split (message, "\n");
      
      for (size_t i=0; i<lines.size (); i++)
      {
        stl::string& line = lines [i];

        if (line.empty ())
          continue;

        write (log_stream, prefix);
        write (log_stream, line);
        write (log_stream, "\r\n");
      }

        //сброс протокола на диск

      log_stream.Buffer ().Flush ();
    }    
    
      //работа с игровым отображением
    void SetView (const GameView& view)
    {
      if (current_view)
      {
        try
        {
          current_view->FlushResources ();                        
        }
        catch (...)
        {
          //подавление всех исключений
        }
      }
      
      current_view = view;                  
      
      if (current_view)
      {
        try
        {
          try
          {
            current_view->LoadResources (scene_player, *device);        

            window.Invalidate (); 
          }
          catch (...)
          {
            current_view->FlushResources ();
            
            current_view = GameView ();
            
            throw;
          }
        }
        catch (std::exception& e)
        {
          LogMessage (format ("Exception at LoadResources: %s", e.what ()).c_str ());
        }
        catch (...)
        {
          LogMessage ("Exception at LoadResources");
        }
      }      
    }

    const GameView& View () const { return current_view; }

    const ::Configuration& Configuration () const
    {
      return configuration;
    }

  private:
      //сброс буфера в файл-протолирования
    size_t LogWriteBuffer (const void* buffer, size_t size)
    {
      size_t result = log_file.Write (buffer, size);

      log_file.Flush ();

      return result;
    }
  
      //обработчик закрытия окна
    void OnClose ()
    {
      syslib::Application::Exit (0);
    }
  
      //обработчик изменения размеров окна
    void OnResize ()
    {
      try
      {
        Viewport vp;
        syslib::Rect client_rect = window.ClientRect ();

        vp.x         = client_rect.left;
        vp.y         = client_rect.top;
        vp.width     = client_rect.right - client_rect.left;
        vp.height    = client_rect.bottom - client_rect.top;
        vp.min_depth = 0;
        vp.max_depth = 1;

        device->RSSetViewport (vp);

        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at window resize: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at window resize");
      }
    }        

      //обработчик перерисовки окна
    void OnRedraw ()
    {
      try
      {
        Color4f clear_color;

        clear_color.red   = 0;
        clear_color.green = 0.7f;
        clear_color.blue  = 0.7f;
        clear_color.alpha = 0;

        device->ClearViews (ClearFlag_All, clear_color, 1.0f, 0);

        if (current_view)
          current_view->OnDraw ();
       
        swap_chain->Present ();    
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at window redraw: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at window redraw");
      }
    }
    
      //обработчик нажатия / отпускания клавиш мыши
    void OnMouse (syslib::WindowEvent event, const syslib::WindowEventContext& context)
    {
      if (!current_view)
        return;

      try
      {
        syslib::Rect client_rect = window.ClientRect ();

        int cursor_x = (context.cursor_position.x - client_rect.left) * current_view->Width () / (client_rect.right - client_rect.left),
            cursor_y = (context.cursor_position.y - client_rect.top) * current_view->Height () / (client_rect.bottom - client_rect.top);

        current_view->OnMouse (event, cursor_x, cursor_y);

//        window.Invalidate ();        
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at process mouse click: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at process mouse click");
      }
    }
    
      //обработчик "холостого хода" приложения
    void OnIdle ()
    {
      if (!current_view)
        return;
        
      try
      {
        current_view->OnIdle ();
        
        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        LogMessage (format ("Exception at idle: %s", e.what ()).c_str ());
      }
      catch (...)
      {
        LogMessage ("Exception at idle");
      }
    }        

  private:
    ::Configuration     configuration;       //настрройки
    OutputFile          log_file;            //файл протолирования
    OutputTextStream    log_stream;          //поток протоколирования работы приложения
    syslib::Window      window;              //главное окно приложения
    SwapChainPtr        swap_chain;          //цепочка обмена главного окна приложения
    DevicePtr           device;              //устройство рендеринга главного окна приложения
    xtl::connection     app_idle_connection; //соединение сигнала обработчика холостого хода приложения
    GameView            current_view;        //текущее игровое отображение
    sound::SoundManager *sound_manager;      //менеджер звуков
    sound::ScenePlayer  *scene_player;       //проигрыватель звуков
};

/*
    Конструктор / деструктор
*/

MyApplication::MyApplication ()
{
  impl = new Impl;
}

MyApplication::~MyApplication ()
{
}

/*
    Получение экземпляра приложения
*/

MyApplication& MyApplication::Instance ()
{
  static MyApplication application;
  
  return application;
}

/*
    Протоколирование
*/

void MyApplication::LogMessage (const char* message)
{
  impl->LogMessage (message);
}

void MyApplication::LogFormatMessage (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  
  VLogFormatMessage (format, list);
}

void MyApplication::VLogFormatMessage (const char* format, va_list list)
{
  if (!format)
    return;

  LogMessage (common::vformat (format, list).c_str ());
}

/*
    Получение количества милисекунд, прошедших от момента запуска приложения
*/

size_t MyApplication::Milliseconds ()
{
  return clock () * 1000 / CLOCKS_PER_SEC;
}

/*
    Установка текущего отображения
*/

void MyApplication::SetView (const GameView& view)
{
  impl->SetView (view);
}

const GameView& MyApplication::View () const
{
  return impl->View ();
}

const ::Configuration& MyApplication::Configuration () const
{
  return impl->Configuration ();
}

