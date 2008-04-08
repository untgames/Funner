#include "shared.h"

using namespace render::low_level;
using namespace common;

namespace
{

/*
    Константы (вынести в конфигурацию)!!!
*/

const char* LOG_FILE_NAME = "water-slides.log"; //имя файла протокола

const size_t   WINDOW_WIDTH  = 400;                         //начальная ширина окна
const size_t   WINDOW_HEIGHT = 300;                         //начальная высота окна
const wchar_t* WINDOW_TITLE  = L"Water slides demo ($Id$)"; //заголовок окна

const size_t FB_COLOR_BITS    = 24; //глубина буфера цвета
const size_t FB_ALPHA_BITS    = 8;  //глубина альфа-буфера
const size_t FB_DEPTH_BITS    = 24; //глубина z-buffer'а
const size_t FB_STENCIL_BITS  = 8;  //глубина буфера трафарета
const size_t FB_BUFFERS_COUNT = 2;  //количество буферов в цепочке обмена

const char* DEVICE_INIT_STRING = ""; //строка инициализации устройства рендеринга

}

/*
    Описание реализации приложения
*/

struct MyApplication::Impl
{
  public:
      //конструктор
    Impl () :
      log_file (LOG_FILE_NAME),
      log_stream (xtl::bind (&Impl::LogWriteBuffer, this, _1, _2)),
      window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT),
      driver (get_opengl_driver ())
    {
      LogMessage ("Create window...");
      
        //установка пользовательской функции отладочного протоколирования

      window.SetDebugLog (xtl::bind (&Impl::LogMessage, this, _1));
      
        //установка заголовка окна
      
      window.SetTitle (WINDOW_TITLE);
      
        //показ окна

      window.Show ();
      
      LogMessage ("Initializing rendering device...");

        //создание цепочки обмена и устройства рендеринга

      SwapChainDesc desc;

      memset (&desc, 0, sizeof (desc));

      desc.frame_buffer.color_bits   = FB_COLOR_BITS;
      desc.frame_buffer.alpha_bits   = FB_ALPHA_BITS;
      desc.frame_buffer.depth_bits   = FB_DEPTH_BITS;
      desc.frame_buffer.stencil_bits = FB_STENCIL_BITS;
      desc.buffers_count             = FB_BUFFERS_COUNT;
      desc.swap_method               = SwapMethod_Discard;
      desc.window_handle             = window.Handle ();
      
      swap_chain = SwapChainPtr (driver->CreateSwapChain (desc), false);
      device     = DevicePtr (driver->CreateDevice (&*swap_chain, DEVICE_INIT_STRING), false);
      
        //оповещение об изменении размеров
      
      OnResize ();

        //регистрация обработчиков событий окна

      window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnRedraw, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Impl::OnResize, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Impl::OnClose, this));

        //регистрация обработчиков событий приложения
        
      app_idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&Impl::OnIdle, this));      
    }
    
      //деструктор
    ~Impl ()
    {
      try
      {
        LogMessage ("Close application");

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
        write (log_stream, "\n");
      }

        //сброс протокола на диск

      log_stream.Buffer ().Flush ();
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

        vp.x         = 0;
        vp.y         = 0;
        vp.width     = window.Width ();
        vp.height    = window.Height ();
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

//        if (redraw)
//          redraw (*this);      
       
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
    
      //обработчик "холостого хода" приложения
    void OnIdle ()
    {
      LogMessage ("Idle");
    }        

  private:
    OutputFile       log_file;   //файл протолирования
    OutputTextStream log_stream; //поток протоколирования работы приложения
    syslib::Window   window;     //главное окно приложения
    DriverPtr        driver;     //OpenGL driver
    SwapChainPtr     swap_chain; //цепочка обмена главного окна приложения
    DevicePtr        device;     //устройство рендеринга главного окна приложения
//    CallbackFn       redraw;     //????
    xtl::connection  app_idle_connection; //соединение сигнала обработчика холостого хода приложения
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

  LogMessage (common::format (format, list).c_str ());
}

/*
    Получение количества милисекунд, прошедших от момента запуска приложения
*/

size_t MyApplication::Milliseconds ()
{
  return clock () * 1000 / CLOCKS_PER_SEC;
}

