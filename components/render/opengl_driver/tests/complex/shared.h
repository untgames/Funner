#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER

#include <cstdio>
#include <cmath>
#include <ctime>
#include <exception>

#include <mathlib.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

//#include <stl/list>
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

#include <media/image.h>
#include <media/mesh.h>

using namespace render::low_level;

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

//тестовое приложение
struct Test
{
  typedef xtl::function<void (Test&)> CallbackFn;

  syslib::Window window;
  DriverPtr      driver;
  SwapChainPtr   swap_chain;
  DevicePtr      device;
  CallbackFn     redraw;

  Test (const wchar_t* title, const CallbackFn& in_redraw, const char* init_string="") :
    window (syslib::WindowStyle_Overlapped, 1024, 768), driver (DriverManager::FindDriver ("OpenGL")), redraw (in_redraw)
  {
    if (!driver)
      throw xtl::format_operation_exception ("Test::Test", "OpenGL driver not found");
  
    window.SetTitle (title);

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 0;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
    desc.fullscreen                = true;
    desc.window_handle             = window.Handle ();

    swap_chain = SwapChainPtr (driver->CreateSwapChain (desc), false);
    device     = DevicePtr (driver->CreateDevice (&*swap_chain, init_string), false);    

    OnResize ();
    
    window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Test::OnRedraw, this));
    window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Test::OnResize, this));
    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&Test::OnClose, this));
  }   
 
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
