#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include <exception>

#include <mathlib.h>

#include <syslib/window.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/string>
#include <stl/vector>

#include <xtl/intrusive_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/connection.h>
#include <xtl/ref.h>
#include <xtl/common_exceptions.h>

#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>

#include <media/image.h>

/*
    Получение смещения поля в структуре (аналог offsetof, компилируемый без предупреждений на gcc)
*/

#ifdef _MSC_VER
  #define TEST_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define TEST_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

using namespace render::low_level;

typedef xtl::com_ptr<IDriver>                  DriverPtr;
typedef xtl::com_ptr<ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<IAdapter>                 AdapterPtr;
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
typedef xtl::com_ptr<IStateBlock>              StateBlockPtr;
typedef xtl::com_ptr<IPredicate>               PredicatePtr;

// режим вывода логов
enum OutputMode
{
  OutputMode_Success     = 1,
  OutputMode_Fail        = 2,
  OutputMode_SuccessOnly = OutputMode_Success,
  OutputMode_FailOnly    = OutputMode_Fail,
  OutputMode_All         = OutputMode_Success | OutputMode_Fail,  

  OutputMode_Default = 0,

  OutputMode_Num
};

//тестовое приложение
struct Test
{
  syslib::Window window;
  SwapChainPtr   swap_chain;
  DevicePtr      device;
  size_t         log_mode;
  
  Test (const char* init_string="") :
    window (syslib::WindowStyle_Overlapped, 640, 480)
  {
    window.SetTitle (L"OpenGL test window");

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 16;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 0;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();
    
    const char* adapter_mask = getenv ("GlAdapterMask");

    DriverManager::CreateSwapChainAndDevice ("OpenGL", adapter_mask ? adapter_mask : "*", desc, init_string, swap_chain, device);    
    
    log_mode = OutputMode_Default;

    char* modestr = getenv ("TestResultOutputMode");

    if (modestr)
    {
      if (!xtl::xstricmp (modestr, "All"))     log_mode = OutputMode_All;
      if (!xtl::xstricmp (modestr, "Success")) log_mode = OutputMode_SuccessOnly;
      if (!xtl::xstricmp (modestr, "Failure")) log_mode = OutputMode_FailOnly;
    }    
  }
};

#endif
