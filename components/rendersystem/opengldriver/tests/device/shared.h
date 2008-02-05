#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <memory.h>
#include <math.h>

#include <exception>

#include <syslib/window.h>

#include <render/low_level/opengl_driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/list>

#include <xtl/intrusive_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/exception.h>

#include <media/image.h> //??!!!!

using namespace render::low_level;

typedef xtl::com_ptr<IDriver>            DriverPtr;
typedef xtl::com_ptr<ISwapChain>         SwapChainPtr;
typedef xtl::com_ptr<IDevice>            DevicePtr;
typedef xtl::com_ptr<ITexture>           TexturePtr;
typedef xtl::com_ptr<IView>              ViewPtr;
typedef xtl::com_ptr<IBlendState>        BlendStatePtr; 
typedef xtl::com_ptr<IDepthStencilState> DepthStencilStatePtr; 

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
    desc.frame_buffer.depth_bits   = 16;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 0;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
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

#endif
