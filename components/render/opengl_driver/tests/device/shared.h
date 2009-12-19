#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <cstdio>
#include <float.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include <exception>

#include <math/matrix.h>

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

#include <media/image.h>

#include <common/hash.h>
#include <common/file.h>

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
//  AdapterPtr     adapter;  
  size_t         log_mode;  
  
  Test (const wchar_t* title, const char* init_string="") :
//    window (syslib::WindowStyle_Overlapped, 640, 480)
    window (syslib::WindowStyle_PopUp, 1024, 768)
  {
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
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();

    DriverManager::CreateSwapChainAndDevice ("OpenGL", "*", desc, init_string, swap_chain, device);    

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

//печать дескриптора blend-state
inline void dump_desc (IBlendState& state)
{
  if (!&state)
  {
    printf ("Null blend state\n");
    
    return;
  }

  using render::low_level::get_name;

  BlendDesc desc;
  
  state.GetDesc (desc);
  
  printf ("Blend state:\n");
  printf ("  blend_enable:                     %s\n", desc.blend_enable ? "true" : "false");
  printf ("  sample_alpha_to_coverage:         %s\n", desc.sample_alpha_to_coverage ? "true" : "false");
  printf ("  blend_color_operation:            %s\n", get_name (desc.blend_color_operation));
  printf ("  blend_color_source_argument:      %s\n", get_name (desc.blend_color_source_argument));
  printf ("  blend_color_destination_argument: %s\n", get_name (desc.blend_color_destination_argument));
  printf ("  blend_alpha_operation:            %s\n", get_name (desc.blend_alpha_operation));  
  printf ("  blend_alpha_source_argument:      %s\n", get_name (desc.blend_alpha_source_argument));
  printf ("  blend_alpha_destination_argument: %s\n", get_name (desc.blend_alpha_destination_argument));  
  printf ("  color_write_mask:                 %s\n", get_name ((ColorWriteFlag)desc.color_write_mask));
}

//печать дескриптора stencil
inline void dump_desc (const StencilDesc& desc)
{
  printf ("    stencil_compare_mode:   %s\n", get_name (desc.stencil_compare_mode));
  printf ("    stencil_fail_operation: %s\n", get_name (desc.stencil_fail_operation));
  printf ("    depth_fail_operation:   %s\n", get_name (desc.depth_fail_operation));
  printf ("    stencil_pass_operation: %s\n", get_name (desc.stencil_pass_operation));
}

//печать дескриптора depth-stencil-state
inline void dump_desc (IDepthStencilState& state)
{
  if (!&state)
  {
    printf ("Null depth-stencil state\n");
  
    return;
  }

  using render::low_level::get_name;
  
  DepthStencilDesc desc;
  
  state.GetDesc (desc);
  
  printf ("Depth-stencil state:\n");
  printf ("  depth_test_enable:   %s\n", desc.depth_test_enable ? "true" : "false");
  printf ("  depth_write_enable:  %s\n", desc.depth_write_enable ? "true" : "false");
  printf ("  stencil_test_enable: %s\n", desc.stencil_test_enable ? "true" : "false");
  printf ("  depth_compare_mode:  %s\n", get_name (desc.depth_compare_mode));
  printf ("  stencil_read_mask:   %02x\n", desc.stencil_read_mask);
  printf ("  stencil_write_mask:  %02x\n", desc.stencil_write_mask);
  printf ("  stencil_desc [FaceMode_Front]:\n");
  dump_desc (desc.stencil_desc [FaceMode_Front]);
  printf ("  stencil_desc [FaceMode_Back]:\n");
  dump_desc (desc.stencil_desc [FaceMode_Back]);  
}

//чтение ихсодного текста шейдера в строку
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);
  
  stl::string buffer (file.Size (), ' ');
  
  file.Read (&buffer [0], file.Size ());
  
  return buffer;
} 

#endif
