#include "shared.h"

void dump_render_targets (const SceneRender& render)
{
  printf ("%u render targets found\n", render.RenderTargetsCount ());
      
  for (size_t i=0; i<render.RenderTargetsCount (); i++)
  {
    RenderTarget rt = render.RenderTarget (i);

    printf (" RT#%u: color-attachment='%s', depth-stencil-attachment='%s'\n", i+1, rt.Attachment (RenderTargetAttachment_Color),
      rt.Attachment (RenderTargetAttachment_DepthStencil));
  }
}

int main ()
{
  printf ("Results of enum_render_targets_test:\n");
  
  try
  {
      //создание рендера сцены

    SceneRender render ("*", "*", "");

      //перебор целей рендеринга
      
    dump_render_targets (render);
    
      //создание цели рендеринга                    
    
    RenderTarget render_target = render.CreateRenderTarget ("FrameBuffer0.Color", "Null.DepthStencil");
    
      //перебор целей рендеринга
      
    dump_render_targets (render);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
