#include "shared.h"

enum Target
{
  Window1_RenderTarget,
  Window1_DepthStencil,
  Window2_RenderTarget,
  Window2_DepthStencil
};

int main ()
{
  printf ("Results of render_targets1_test:\n");
  
  try
  {
    ViewPtr view [4];
    
    {    
      Test           test (L"OpenGL device test first window (render_targets1)");
      syslib::Window window (syslib::WindowStyle_Overlapped, 400, 200);
      
      window.SetTitle (L"OpenGL device test second window (render_targets1)");
      
      SwapChainDesc swap_chain_desc;
      
      test.swap_chain->GetDesc (swap_chain_desc);
      
      swap_chain_desc.window_handle = window.Handle ();

      SwapChainPtr swap_chain (test.driver->CreateSwapChain (swap_chain_desc), false);    
      
      TexturePtr texture [4] = {
        TexturePtr (test.device->CreateRenderTargetTexture (test.swap_chain.get (), 1), false),
        TexturePtr (test.device->CreateDepthStencilTexture (test.swap_chain.get ()), false),
        TexturePtr (test.device->CreateRenderTargetTexture (swap_chain.get (), 1), false),
        TexturePtr (test.device->CreateDepthStencilTexture (swap_chain.get ()), false)
      };    
      
      ViewDesc view_desc;

      memset (&view_desc, 0, sizeof (view_desc));

      view_desc.layer     = 0;
      view_desc.mip_level = 0;

      for (int i=0; i<4; i++)
        view [i] = ViewPtr (test.device->CreateView (texture [i].get (), view_desc), false);

      test.device->OSSetRenderTargets (view [Window1_RenderTarget].get (), view [Window2_DepthStencil].get ());
      
      Color4f clear_color;
      
      clear_color.red   = 0;
      clear_color.green = 0.7f;
      clear_color.blue  = 0.7f;
      clear_color.alpha = 0;

      test.device->ClearViews (ClearFlag_All, clear_color, 0.5f, 12);
      test.device->Draw (PrimitiveType_PointList, 0, 0);

      test.swap_chain->Present ();      
      swap_chain->Present ();

//      syslib::Application::Run ();
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
