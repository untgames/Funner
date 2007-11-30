#include "shared.h"

using namespace syslib;

typedef xtl::com_ptr<ISwapChain> SwapChainPtr;
typedef xtl::com_ptr<IDevice>    DevicePtr;

int main ()
{
  printf ("Results of device_info_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 200);
    
    window.SetTitle ("OpenGL device test window");
    
    IDriver* driver = get_opengl_driver ();    
    
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

    SwapChainPtr swap_chain (driver->CreateSwapChain (desc), false);
    DevicePtr    device (driver->CreateDevice (&*swap_chain), false);

    printf ("Device: '%s'\n", device->GetName ());

    IPropertyList* properties = device->GetProperties ();

    for (size_t j=0; j<properties->GetSize (); j++)
      printf ("  %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
