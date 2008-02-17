#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include <exception>

#include <mathlib.h>

#include <syslib/window.h>
#include <syslib/application.h> //???

#include <render/low_level/opengl_driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

//#include <stl/list>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/connection.h>
#include <xtl/ref.h>

#include <common/file.h>
#include <common/exception.h>
#include <common/hash.h>

#include <media/image.h> //??!!!!

using namespace render::low_level;

typedef xtl::com_ptr<IDriver>                 DriverPtr;
typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;
typedef xtl::com_ptr<IDevice>                 DevicePtr;
typedef xtl::com_ptr<ITexture>                TexturePtr;
typedef xtl::com_ptr<IView>                   ViewPtr;
typedef xtl::com_ptr<IBlendState>             BlendStatePtr;
typedef xtl::com_ptr<IDepthStencilState>      DepthStencilStatePtr;
typedef xtl::com_ptr<IInputLayout>            InputLayoutPtr;
typedef xtl::com_ptr<IBuffer>                 BufferPtr;
typedef xtl::com_ptr<IRasterizerState>        RasterizerStatePtr;
typedef xtl::com_ptr<IProgram>                ProgramPtr;
typedef xtl::com_ptr<IShaderParametersLayout> ShaderParametersLayoutPtr;

//тестовое приложение
struct Test
{
  syslib::Window window;
  DriverPtr      driver;
  SwapChainPtr   swap_chain;
  DevicePtr      device;
  
  Test (const wchar_t* title, const char* init_string="") :
    window (syslib::WindowStyle_Overlapped, 400, 200), driver (get_opengl_driver ())
  {
    window.SetTitle (title);
//    window.Hide ();

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 24;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
//    desc.samples_count             = 0;
    desc.samples_count             = 6;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();

    swap_chain = SwapChainPtr (driver->CreateSwapChain (desc), false);
    device     = DevicePtr (driver->CreateDevice (&*swap_chain, init_string), false);
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

//печать дескриптора rasterizer-state
inline void dump_desc (IRasterizerState& state)
{
  if (!&state)
  {
    printf ("Null rasterizer state\n");
    
    return;
  }

  using render::low_level::get_name;

  RasterizerDesc desc;
  
  state.GetDesc (desc);
  
  printf ("Rasterizer state:\n");
  printf ("  fill_mode:               %s\n", get_name (desc.fill_mode));
  printf ("  cull_mode:               %s\n", get_name (desc.cull_mode));
  printf ("  front_counter_clockwise: %s\n", desc.front_counter_clockwise ? "true" : "false");
  printf ("  depth_bias:              %d\n", desc.depth_bias);
  printf ("  scissor_enable:          %s\n", desc.scissor_enable ? "true" : "false");
  printf ("  multisample_enable:      %s\n", desc.multisample_enable ? "true" : "false");  
  printf ("  antialiased_line_enable: %s\n", desc.antialiased_line_enable ? "true" : "false");
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

#endif
