#include "shared.h"

int main ()
{
  try
  {
    printf ("Results of render_targets2_test:\n");
    
    Test test (L"OpenGL device test window (render_targets2)");
    
    TextureDesc bindable_texture_desc;

    memset (&bindable_texture_desc, 0, sizeof (bindable_texture_desc));
    
    bindable_texture_desc.dimension  = TextureDimension_2D;
    bindable_texture_desc.width      = 512;
    bindable_texture_desc.height     = 512;
    bindable_texture_desc.layers     = 1;
    bindable_texture_desc.format     = PixelFormat_RGBA8;
    bindable_texture_desc.bind_flags = BindFlag_Texture;
    
    TexturePtr bindable_color_texture (test.device->CreateTexture (bindable_texture_desc), false),    
               back_buffer_texture (test.device->CreateRenderTargetTexture (test.swap_chain.get (), 1), false),
               depth_stencil_buffer_texture (test.device->CreateDepthStencilTexture (test.swap_chain.get ()), false);
               
    bindable_texture_desc.format = PixelFormat_D24X8;
    
    TexturePtr bindable_depth_texture (test.device->CreateTexture (bindable_texture_desc), false);
    
    bindable_texture_desc.format = PixelFormat_D24S8;
    
    TexturePtr bindable_depth_stencil_texture (test.device->CreateTexture (bindable_texture_desc), false);    
       
    bindable_texture_desc.bind_flags = BindFlag_RenderTarget;
    bindable_texture_desc.format     = PixelFormat_RGBA8;
    
    TexturePtr color_texture (test.device->CreateTexture (bindable_texture_desc), false);
    
    bindable_texture_desc.bind_flags = BindFlag_DepthStencil;
    bindable_texture_desc.format     = PixelFormat_D24X8;
    
    TexturePtr depth_texture (test.device->CreateTexture (bindable_texture_desc), false);
    
    static const int VIEWS_COUNT = 8;
    
    ViewPtr  view [VIEWS_COUNT];  
    ViewDesc view_desc;
    
    memset (&view_desc, 0, sizeof (view_desc));
    
    view [0] = ViewPtr (test.device->CreateView (bindable_color_texture.get (), view_desc), false);
    view [1] = ViewPtr (test.device->CreateView (bindable_depth_texture.get (), view_desc), false);    
    view [2] = ViewPtr (test.device->CreateView (bindable_depth_stencil_texture.get (), view_desc), false);
    view [3] = ViewPtr (test.device->CreateView (back_buffer_texture.get (), view_desc), false);
    view [4] = ViewPtr (test.device->CreateView (depth_stencil_buffer_texture.get (), view_desc), false);        
    view [5] = ViewPtr (test.device->CreateView (color_texture.get (), view_desc), false);
    view [6] = ViewPtr (test.device->CreateView (depth_texture.get (), view_desc), false);
    view [7] = 0;

    bool status [VIEWS_COUNT][VIEWS_COUNT];
    
    static const char* view_names [] = {
      "RGBA texture",
      "D24X8 texture",
      "D24S8 texture",
      "Swap chain color buffer",
      "Swap chain depth-stencil buffer",
      "Auxilary color buffer",
      "Auxilary depth-stencil buffer",
      "Null view"
    };
    
    memset (&status [0][0], 0, sizeof (status));
    
    for (int i=0; i<VIEWS_COUNT; i++)
      for (int j=0; j<VIEWS_COUNT; j++)
      {
        printf ("set render targets color-buffer=%s, depth-stencil-buffer=%s: ", view_names [i], view_names [j]);
        
        try
        {
          test.device->OSSetRenderTargets (view [i].get (), view [j].get ());
          test.device->Draw (PrimitiveType_PointList, 0, 0);
          
          status [i][j] = true;
          
          printf ("Ok\n");
        }
        catch (std::exception& exception)
        {
          printf ("Fail!\n%s\n", exception.what ());
        }
      }

    printf ("Status info:\n");

    for (int i=0; i<VIEWS_COUNT; i++)
      printf ("  %d) %s\n", i+1, view_names [i]);

    printf ("Status table (rows - render-target view, columns - depth-stencil view):\n");
    printf ("  12345678\n");
          
    for (int i=0; i<VIEWS_COUNT; i++)
    {
      printf ("%d|", i+1);
      
      for (int j=0; j<VIEWS_COUNT; j++)
        printf (status [i][j] ? "+" : "-");
        
      printf ("\n");
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
