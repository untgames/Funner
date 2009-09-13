#include "shared.h"

using namespace syslib;

typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

void print_log (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

void dump (ISwapChain& swap_chain)
{
  SwapChainDesc desc;
  
  swap_chain.GetDesc (desc);  

  printf ("Swap chain description:\n");
  printf ("    Adapter name:  '%s'\n", swap_chain.GetAdapter ()->GetName ());
  printf ("    Adapter path:  '%s'\n", swap_chain.GetAdapter ()->GetPath ());
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
  
  IPropertyList* properties = swap_chain.GetProperties ();        

  for (size_t j=0; j<properties->GetSize (); j++)
    printf ("      %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));  
}

int main ()
{
  printf ("Results of swap_chain1_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.*", &print_log);
    
    Window window (WindowStyle_Overlapped, 800, 600);
    
    window.SetTitle ("OpenGL driver test window");
//    window.SetPosition (1280 - 400 / 2-1, 200);    
    
    xtl::com_ptr<IDriver> driver = DriverManager::FindDriver ("OpenGL");
    
    if (!driver || !driver->GetAdaptersCount ())
    {
      printf ("OpenGL driver not found");
      return 0;
    }
    
    SwapChainDesc desc;
    
    memset (&desc, 0, sizeof (desc));

    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.alpha_bits   = 8;
    desc.frame_buffer.depth_bits   = 16;
    desc.frame_buffer.stencil_bits = 0;
    desc.buffers_count             = 1;
    desc.samples_count             = 0;
    desc.swap_method               = SwapMethod_Discard;
    desc.vsync                     = true;
    desc.fullscreen                = false;
    desc.window_handle             = window.Handle ();

    typedef stl::vector<IAdapter*> AdapterArray;

    AdapterArray adapters (driver->GetAdaptersCount ());

    for (size_t i=0; i<adapters.size (); i++)
      adapters [i] = driver->GetAdapter (i);

    SwapChainPtr swap_chain (driver->CreateSwapChain (adapters.size (), &adapters [0], desc), false);

//    printf ("Swap chain containing output: '%s'\n", swap_chain->GetContainingOutput ()->GetName ());

    dump (*swap_chain);
    
    printf ("Driver:\n");
    printf ("  description:   '%s'\n", driver->GetDescription ());
    printf ("  adapters count: %u\n", driver->GetAdaptersCount ());

    for (size_t i=0; i<driver->GetAdaptersCount (); i++)
    {
      IAdapter*      adapter    = driver->GetAdapter (i);
      IPropertyList* properties = adapter->GetProperties ();

      printf ("  adapter #%u:\n", i);
      printf ("    name:        '%s'\n", adapter->GetName ());
      printf ("    path:        '%s'\n", adapter->GetPath ());
      printf ("    description: '%s'\n", adapter->GetDescription ());

      for (size_t j=0; j<properties->GetSize (); j++)
        printf ("    %s: '%s'\n", properties->GetKey (j), properties->GetValue (j));

      printf ("    outputs count: %u\n", adapter->GetOutputsCount ());

      for (size_t j=0; j<adapter->GetOutputsCount (); j++)
      {
        IOutput* output = adapter->GetOutput (j);

        printf ("    output #%u:\n", j);
        printf ("      name: '%s'\n", output->GetName ());        
      }
    }    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
