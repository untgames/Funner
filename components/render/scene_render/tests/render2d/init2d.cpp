#include "shared.h"

int main ()
{
  printf ("Results of init2d_test:\n");

  try
  {
    SceneRender render ("Debug", "Renderer2d", "Render2d");    
    
    printf ("renderer: %s\n", render.RendererDescription ());
    printf ("paths:    %s\n", render.RenderPaths ());

    const Rect& window = render.Window ();
    
    printf ("window:   left=%u top=%u width=%u height=%u\n", window.left, window.top, window.width, window.height);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
