#ifndef RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_DEVICE_TESTS_SHARED_HEADER

#include <stdio.h>
#include <memory.h>

#include <exception>

#include <syslib/window.h>

#include <render/low_level/opengl_driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>

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

    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 16;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 4;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = false;
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();
    
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
  printf ("  width:        %u\n", desc.width);
  printf ("  height:       %u\n", desc.height);
  printf ("  color_bits:   %u\n", desc.color_bits);
  printf ("  alpha_bits:   %u\n", desc.alpha_bits);
  printf ("  depth_bits:   %u\n", desc.depth_bits);
  printf ("  stencil_bits: %u\n", desc.stencil_bits);
}

//печать BlendArgument
inline void dump (BlendArgument argument)
{
  switch (argument)
  {
    case BlendArgument_Zero:                    printf ("BlendArgument_Zero"); break;
    case BlendArgument_One:                     printf ("BlendArgument_One"); break;
    case BlendArgument_SourceColor:             printf ("BlendArgument_SourceColor"); break;
    case BlendArgument_SourceAlpha:             printf ("BlendArgument_SourceAlpha"); break;
    case BlendArgument_InverseSourceColor:      printf ("BlendArgument_InverseSourceColor"); break;
    case BlendArgument_InverseSourceAlpha:      printf ("BlendArgument_InverseSourceAlpha"); break;
    case BlendArgument_DestinationColor:        printf ("BlendArgument_DestinationColor"); break;
    case BlendArgument_DestinationAlpha:        printf ("BlendArgument_DestinationAlpha"); break;
    case BlendArgument_InverseDestinationColor: printf ("BlendArgument_InverseDestinationColor"); break;
    case BlendArgument_InverseDestinationAlpha: printf ("BlendArgument_InverseDestinationAlpha"); break;
    default:                                    printf ("unknown");
  }
}

//печать дескриптора blend-state
inline void dump_desc (IBlendState& state)
{
  BlendDesc desc;
  
  state.GetDesc (desc);
  
  printf ("Blend state:\n");
  
  printf ("  blend_enable:               %s\n", desc.blend_enable ? "true" : "false");
  printf ("  blend_operation:            ");
  
  switch (desc.blend_operation)
  {
    case BlendOperation_Add:                printf ("BlendOperation_Add");         break;
    case BlendOperation_Subtraction:        printf ("BlendOperation_Subtraction"); break;
    case BlendOperation_ReverseSubtraction: printf ("BlendOperation_ReverseSubtraction"); break;
    case BlendOperation_Min:                printf ("BlendOperation_Min"); break;
    case BlendOperation_Max:                printf ("BlendOperation_Max"); break;
    default:                                printf ("unknown"); break;
  }
  
  printf ("\n");
  printf ("  blend_source_argument:      ");
  dump   (desc.blend_source_argument);
  printf ("\n  blend_destination_argument: ");
  dump   (desc.blend_destination_argument);
  printf ("\n  color_write_mask:           ");
  
  if (desc.color_write_mask & ColorWriteFlag_Red)   printf ("red ");
  if (desc.color_write_mask & ColorWriteFlag_Green) printf ("green ");
  if (desc.color_write_mask & ColorWriteFlag_Blue)  printf ("blue ");
  if (desc.color_write_mask & ColorWriteFlag_Alpha) printf ("alpha");
  
  printf ("\n");
}

#endif
