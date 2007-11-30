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

#endif
