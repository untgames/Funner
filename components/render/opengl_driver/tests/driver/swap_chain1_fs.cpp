#include "shared.h"

using namespace syslib;

typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

void dump (const SwapChainDesc& desc)
{
  printf ("Swap chain description:\n");
  printf ("    Color bits:    %u\n", desc.frame_buffer.color_bits);
  printf ("    Alpha bits:    %u\n", desc.frame_buffer.alpha_bits);
  printf ("    Depth bits:    %u\n", desc.frame_buffer.depth_bits);
  printf ("    Stencil bits:  %u\n", desc.frame_buffer.stencil_bits);
  printf ("    Buffers count: %u\n", desc.buffers_count);
  printf ("    Samples count: %u\n", desc.samples_count);
  printf ("    Swap method:   ");

  switch (desc.swap_method)
  {
    case SwapMethod_Discard: printf ("discard\n"); break;
    case SwapMethod_Flip:    printf ("flip\n"); break;
    case SwapMethod_Copy:    printf ("copy\n"); break;
    default:                 printf ("unknown\n"); break;
  }

  printf ("    VSync:         %s\n", desc.vsync ? "enable" : "disable");
  printf ("    FullScreen:    %s\n", desc.fullscreen ? "enable" : "disable");
}

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  common::LogFilter log_filter ("*", &log_print);

  printf ("Results of swap_chain1_fs_test:\n");

  try
  {
    Window window (WindowStyle_Overlapped, 1280, 1024);

    window.SetTitle ("OpenGL driver test window");
//    window.SetPosition (1280 - 400 / 2-1, 200);

    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");

    if (!driver || !driver->GetAdaptersCount ())
    {
      printf ("OpenGL driver not found\n");
      return 0;
    }

    SwapChainDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 32;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 16;
    desc.frame_buffer.stencil_bits = 8;
    desc.buffers_count             = 2;
    desc.samples_count             = 4;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.fullscreen                = true;
    desc.window_handle             = window.Handle ();

    typedef stl::vector<IAdapter*> AdapterArray;

    AdapterArray adapters (driver->GetAdaptersCount ());

    for (size_t i=0; i<adapters.size (); i++)
      adapters [i] = driver->GetAdapter (i);

    SwapChainPtr swap_chain (driver->CreateSwapChain (adapters.size (), &adapters [0], desc), false);

    printf ("Swap chain containing output: '%s'\n", swap_chain->GetContainingOutput ()->GetName ());

    swap_chain->GetDesc (desc);

    dump (desc);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
