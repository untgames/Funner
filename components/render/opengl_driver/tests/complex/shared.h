#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER

#include <cstdio>
#include <cmath>
#include <exception>

#include <mathlib.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/string>
#include <stl/hash_map>

#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/connection.h>
#include <xtl/ref.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/time.h>

#include <media/compressed_image.h>
#include <media/image.h>
#include <media/mesh.h>

using namespace render::low_level;

/*
    Получение смещения поля в структуре (аналог offsetof, компилируемый без предупреждений на gcc)
*/

#ifdef _MSC_VER
  #define TEST_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define TEST_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

typedef xtl::com_ptr<IDriver>                  DriverPtr;
typedef xtl::com_ptr<ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<IDevice>                  DevicePtr;
typedef xtl::com_ptr<ITexture>                 TexturePtr;
typedef xtl::com_ptr<ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<IView>                    ViewPtr;
typedef xtl::com_ptr<IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<IBuffer>                  BufferPtr;
typedef xtl::com_ptr<IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<IProgram>                 ProgramPtr;
typedef xtl::com_ptr<IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<IPredicate>               PredicatePtr;

//протокол теста
struct TestLogFilter
{
  common::LogFilter log_filter;

  TestLogFilter () : log_filter ("*", &LogPrint) {}

  static void LogPrint (const char* log_name, const char* message)
  {
    printf ("%s: %s\n", log_name, message);
    fflush (stdout);
  }
};

typedef common::Singleton<TestLogFilter> TestLogFilterSingleton;

//тестовое приложение
struct Test
{
  typedef xtl::function<void (Test&)> CallbackFn;

  syslib::Window     window;
  SwapChainPtr       swap_chain;
  DevicePtr          device;
  CallbackFn         redraw;

  Test (const wchar_t* title, const CallbackFn& in_redraw, const char* adapter_mask="*", const char* init_string="") :
    window (syslib::WindowStyle_Overlapped, 800, 600),
    redraw (in_redraw)
  {
    TestLogFilterSingleton::Instance (); //инициализация фильтра протокольных сообщений

    window.SetTitle (title);
    window.Show ();

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 32;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 4;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
    desc.window_handle             = window.Handle ();

    DriverManager::CreateSwapChainAndDevice ("OpenGL", adapter_mask, desc, init_string, swap_chain, device);
    
//    swap_chain->SetFullscreenState (true);

    OnResize ();

    window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Test::OnRedraw, this));
    window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Test::OnResize, this));
    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnClose, this));

    window.Invalidate ();
  }

  void OnResize ()
  {
    try
    {
      syslib::Rect rect = window.ClientRect ();

      Viewport vp;

      vp.x         = rect.left;
      vp.y         = rect.top;
      vp.width     = rect.right - rect.left;
      vp.height    = rect.bottom - rect.top;
      vp.min_depth = 0;
      vp.max_depth = 1;

      device->RSSetViewport (vp);
    }
    catch (std::exception& e)
    {
      printf ("resize exception: %s\n", e.what ());
    }
  }

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

      if (redraw)
        redraw (*this);

      device->Flush ();

      swap_chain->Present ();      
    }
    catch (std::exception& e)
    {
      printf ("redraw exception: %s\n", e.what ());
    }
  }

  void OnClose ()
  {
    syslib::Application::Exit (0);
  }
};

//чтение ихсодного текста шейдера в строку
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], file.Size ());

  return buffer;
}


#endif
