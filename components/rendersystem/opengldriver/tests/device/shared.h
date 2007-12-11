#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <memory.h>

#include <exception>

#include <syslib/window.h>

#include <render/low_level/opengl_driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <xtl/intrusive_ptr.h>

using namespace render::low_level;

typedef xtl::com_ptr<IDriver>    DriverPtr;
typedef xtl::com_ptr<ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<IDevice>    DevicePtr;

//тестовое приложение
struct Test
{
  syslib::Window window;
  DriverPtr      driver;
  SwapChainPtr   swap_chain;  
  DevicePtr      device;
  
  Test (const char* title) : window (syslib::WindowStyle_Overlapped, 400, 200), driver (get_opengl_driver ())
  {
    window.SetTitle (title);

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits          = 24;
    desc.frame_buffer.alpha_bits          = 8;
    desc.frame_buffer.depth_bits          = 16;
    desc.frame_buffer.stencil_bits        = 8;
    desc.frame_buffer.color_buffers_count = 4;
    desc.buffers_count                    = 2;
    desc.samples_count                    = 4;
    desc.swap_method                      = SwapMethod_Discard;
    desc.vsync                            = false;
    desc.fullscreen                       = false;
    desc.window_handle                    = window.Handle ();
    
    swap_chain = SwapChainPtr (driver->CreateSwapChain (desc), false);
    device     = DevicePtr (driver->CreateDevice (&*swap_chain), false);
  }
};

//печать дескриптора FrameBuffer
inline void dump_desc (IFrameBuffer& buffer)
{
  FrameBufferDesc desc;
  
  buffer.GetDesc (desc);
  
  printf ("Frame buffer:\n");
  printf ("  width:         %u\n", desc.width);
  printf ("  height:        %u\n", desc.height);
  printf ("  color_bits:    %u\n", desc.color_bits);
  printf ("  alpha_bits:    %u\n", desc.alpha_bits);
  printf ("  depth_bits:    %u\n", desc.depth_bits);
  printf ("  stencil_bits:  %u\n", desc.stencil_bits);
  printf ("  color_buffers: %u\n", desc.color_buffers_count);
}

//печать дескриптора blend-state
inline void dump_desc (IBlendState& state)
{
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

#endif
