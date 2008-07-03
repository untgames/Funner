#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;

typedef xtl::com_ptr<render::low_level::IDriver> DriverPtr;
typedef xtl::com_ptr<ISwapChain>                 SwapChainPtr;
typedef xtl::com_ptr<IDevice>                    DevicePtr;

int main ()
{
  try
  {
    printf ("Low level driver name is '%s'\n", LowLevelDriver::Name ());

    render::mid_level::IDriver *low_level_driver = LowLevelDriver::Driver ();

    printf ("Driver description: '%s'\n", low_level_driver->GetDescription ());
    printf ("Renderers count is %u\n", low_level_driver->GetRenderersCount ());

    printf ("Renderers: ");
    for (size_t i = 0; i < low_level_driver->GetRenderersCount (); i++)
      printf ("%s; ", low_level_driver->GetRendererName (i));
    printf ("\n");

    syslib::Window window (syslib::WindowStyle_Overlapped, 1024, 768);

    SwapChainPtr   swap_chain;
    DevicePtr      device;
    SwapChainDesc  desc;

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
    
    render::low_level::DriverManager::CreateSwapChainAndDevice ("*", desc, "", swap_chain, device);

    LowLevelDriver::RegisterRenderer ("MyRenderer", device.get (), swap_chain.get ());

    xtl::com_ptr<IRenderer> renderer (render::mid_level::DriverManager::CreateRenderer (LowLevelDriver::Name (), "MyRenderer"), false);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
