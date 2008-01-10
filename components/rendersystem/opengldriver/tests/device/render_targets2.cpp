#include "shared.h"

void short_dump (ITexture* texture)
{
  TextureDesc desc;
  
  texture->GetDesc (desc);
  
  printf ("bind_flags=%s, format=%s", get_name ((BindFlag)desc.bind_flags), get_name (desc.format));
}

void short_dump (IView* view)
{
  if (view) short_dump (view->GetTexture ());
  else      printf ("null");
}

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
               
//    bindable_texture_desc.format = PixelFormat_D24X8;
    
//    TexturePtr bindable_depth_texture (test.device->CreateTexture (bindable_texture_desc), false);
    
    bindable_texture_desc.bind_flags = BindFlag_RenderTarget;
    bindable_texture_desc.format     = PixelFormat_RGBA8;    
    
    TexturePtr color_texture (test.device->CreateTexture (bindable_texture_desc), false);
    
    static const int VIEWS_COUNT = 5;
    
    ViewPtr  view [VIEWS_COUNT];  
    ViewDesc view_desc;
    
    memset (&view_desc, 0, sizeof (view_desc));
    
    view [0] = ViewPtr (test.device->CreateView (bindable_color_texture.get (), view_desc), false);
    view [1] = ViewPtr (test.device->CreateView (back_buffer_texture.get (), view_desc), false);
    view [2] = ViewPtr (test.device->CreateView (depth_stencil_buffer_texture.get (), view_desc), false);        
    view [3] = ViewPtr (test.device->CreateView (color_texture.get (), view_desc), false);
    view [4] = 0;
//    view [2] = ViewPtr (test.device->CreateView (bindable_depth_texture.get (), view_desc), false);    
    
    for (int i=0; i<VIEWS_COUNT; i++)
      for (int j=0; j<VIEWS_COUNT; j++)
      {
        printf     ("set render targets color-buffer={");
        short_dump (view [i].get ());
        printf     ("}, depth-stencil={");
        short_dump (view [j].get ());
        printf     ("}: ");
        
        try
        {
          test.device->OSSetRenderTargets (view [i].get (), view [j].get ());
          test.device->Draw (PrimitiveType_PointList, 0, 0);
          printf ("Ok\n");
        }
        catch (std::exception& exception)
        {
          printf ("\n%s\n", exception.what ());
        }
      }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
