#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_TESTS_SHARED_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_TESTS_SHARED_HEADER

#include <cstdio>
#include <cstring>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>

#include <common/log.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <render/mid_level/driver.h>
#include <render/mid_level/low_level_driver.h>
#include <render/mid_level/renderer2d.h>

#include <media/image.h>

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace math;

/*
    Константы
*/

const size_t WINDOW_WIDTH       = 1024;                        //ширина окна
const size_t WINDOW_HEIGHT      = 768;                         //высота окна
const char*  DEVICE_INIT_STRING = "max_version=1.1 disable=*"; //строка инициализация низкоуровневого устройства отрисовки

/*
    Переопределения типов
*/

typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
typedef xtl::com_ptr<render::mid_level::IRenderer>  RendererPtr;
typedef xtl::com_ptr<renderer2d::IRenderer>         Renderer2dPtr;
typedef xtl::com_ptr<IClearFrame>                   ClearFramePtr;
typedef xtl::com_ptr<renderer2d::IFrame>            FramePtr;
typedef xtl::com_ptr<ITexture>                      TexturePtr;
typedef xtl::com_ptr<IPrimitive>                    PrimitivePtr;

/*
    Тестовое приложение
*/

class BasicTest
{
  public:
///Конструктор  
    BasicTest (const wchar_t* title) : window (syslib::WindowStyle_Overlapped, WINDOW_WIDTH, WINDOW_HEIGHT)
    {
        //подписка на события протоколирования

      common::LogSystem::RegisterLogHandler ("*", &LogMessage);

        //инициализация окна
    
      window.SetTitle (title);
      window.Show ();

        //создание устройства отрисовки

      render::low_level::SwapChainDesc desc;

      memset (&desc, 0, sizeof (desc));

      desc.frame_buffer.color_bits = 16;
      desc.frame_buffer.alpha_bits = 0;
      desc.frame_buffer.depth_bits = 16;
      desc.buffers_count           = 2;
      desc.swap_method             = render::low_level::SwapMethod_Discard;
      desc.window_handle           = window.Handle ();

      render::low_level::DriverManager::CreateSwapChainAndDevice ("*", "*", desc, DEVICE_INIT_STRING, swap_chain, device);
      
        //регистрация системы визуализации
        
      render::low_level::ISwapChain* swap_chains []    = {swap_chain.get ()};
      size_t                         swap_chains_count = sizeof (swap_chains) / sizeof (*swap_chains);

      LowLevelDriver::RegisterRenderer ("MyRenderer", device.get (), swap_chains_count, swap_chains);
      
        //создание системы визуализации
        
      RendererPtr basic_renderer (DriverManager::CreateRenderer (LowLevelDriver::Name (), "MyRenderer"), false);

      renderer = Renderer2dPtr (dynamic_cast<renderer2d::IRenderer*> (basic_renderer.get ()));

      if (!renderer)
        throw xtl::format_operation_exception ("BasicTest::Test", "No renderer2d");
        
        //создание очищающего кадра
        
      clear_frame = ClearFramePtr (renderer->CreateClearFrame (), false);

      clear_frame->SetRenderTargets (renderer->GetColorBuffer (0), renderer->GetDepthStencilBuffer (0));
      clear_frame->SetFlags         (render::mid_level::ClearFlag_All);
      clear_frame->SetColor         (math::vec4f (0.7f, 0.f, 0.f, 0.f));
      
        //создание базового кадра

      frame = FramePtr (renderer->CreateFrame (), false);

      frame->SetRenderTargets (renderer->GetColorBuffer (0), renderer->GetDepthStencilBuffer (0));
      frame->SetProjection    (GetOrthoProjectionMatrix (-100, 100, -100, 100, -1000, 1000));

        //регистрация обработчиков событий окна

      window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&BasicTest::OnRedraw, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnSize,  xtl::bind (&BasicTest::OnResize, this));
      window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&BasicTest::OnClose, this));

        //установка размеров области вывода

      OnResize ();      
    }    
    
///Получение объектов визуализации
    renderer2d::IRenderer* Renderer () { return renderer.get (); }
    renderer2d::IFrame*    Frame    () { return frame.get (); }
    
///Загрузка ресурсов
    TexturePtr LoadTexture (const char* file_name)
    {
      if (!file_name)
        throw xtl::make_null_argument_exception ("BasicTest::LoadTexture", "file_name");
    
      media::Image image (file_name, media::PixelFormat_RGBA8);

      return TexturePtr (renderer->CreateTexture (image), false);
    }
    
///Выполнение теста
    int Run ()
    {
      syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&BasicTest::OnIdleWrapper, this));

      syslib::Application::Run ();    
      
      return syslib::Application::GetExitCode ();
    }
    
  private:
///Обработчик событий главного цикла приложения
    virtual void OnIdle () {}
    
///Обёртка над обработчиком событий главного цикла приложения
    void OnIdleWrapper ()
    {
      try
      {
        OnIdle ();
        
        Redraw ();
      }
      catch (std::exception& exception)
      {
        printf ("exception at idle: %s\n", exception.what ());
      }
    }

///Перерисовка
    void Redraw ()
    {
      try
      {      
        static size_t last_fps = 0, frames_count = 0;
        
          //замер количества кадров в секунду

        if (clock () - last_fps > CLK_TCK)
        {
          printf ("FPS: %.2f\n", float (frames_count)/float (clock () - last_fps)*float (CLK_TCK));

          last_fps     = clock ();
          frames_count = 0;
        }                  
        
          //рисование кадров
          
        renderer->AddFrame   (clear_frame.get ());        
        renderer->AddFrame   (frame.get ());
        renderer->DrawFrames ();        
        
          //увеличение числа кадров
        
        frames_count++;        
      }
      catch (std::exception& e)
      {
        printf ("redraw exception: %s\n", e.what ());
      }          
    }    
  
  private:
///Протоколирование
    static void LogMessage (const char* name, const char* message)
    {
      printf ("Log message from log '%s': '%s'\n", name, message);
    }

///Обработчик события изменения размеров окна
    void OnResize ()
    {
      try
      {
        syslib::Rect client_rect = window.ClientRect ();      

        render::mid_level::Viewport viewport;

        viewport.x      = client_rect.left;
        viewport.y      = client_rect.top;
        viewport.width  = client_rect.right - client_rect.left;
        viewport.height = client_rect.bottom - client_rect.top;

        frame->SetViewport (viewport);

        window.Invalidate ();
      }
      catch (std::exception& e)
      {
        printf ("resize exception: %s\n", e.what ());
      }
    }

///Обработчик события перерисовки окна
    void OnRedraw ()
    {
      Redraw ();
    }

///Обработчик события закрытия окна
    void OnClose ()
    {
      syslib::Application::Exit (0);
    }
    
///Получение ортографической матрицы проекции
    static math::mat4f GetOrthoProjectionMatrix (float left, float right, float bottom, float top, float znear, float zfar)
    {
      math::mat4f proj_matrix;

      float width  = right - left,
            height = top - bottom,
            depth  = zfar - znear;  

        //выбрана матрица проецирования, используемая gluOrtho2D

      proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, - (right + left) / width);
      proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
      proj_matrix [2] = math::vec4f (0, 0, 2.0f / depth, - (znear + zfar) / depth);
      proj_matrix [3] = math::vec4f (0, 0, 0, 1);
      
      return proj_matrix;
    }    

  private:
    syslib::Window window;
    SwapChainPtr   swap_chain;
    DevicePtr      device;
    Renderer2dPtr  renderer;
    ClearFramePtr  clear_frame;
    FramePtr       frame;
};

//получение случайного числа в диапазоне [min_value;max_value]
float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

#endif
