#include "shared.h"

int main ()
{
  printf ("Results of init2d_test:\n");

  try
  {
    SceneRender render ("Debug", "Renderer2d", "Render2d");    
    
    printf ("renderer: %s\n", render.RendererDescription ());
    printf ("paths:    %s\n", render.RenderPaths ());
    
    RenderTarget render_target = render.CreateRenderTarget ("default", "default");    

    const Rect& screen_window     = render_target.ScreenArea ();
    const Rect& renderable_window = render_target.RenderableArea ();    
    
    printf ("screen:     left=%u top=%u width=%u height=%u\n", screen_window.left, screen_window.top, screen_window.width, screen_window.height);
    printf ("renderable: left=%u top=%u width=%u height=%u\n", renderable_window.left, renderable_window.top, renderable_window.width, renderable_window.height);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
