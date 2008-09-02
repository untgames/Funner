#include "shared.h"

int main ()
{
  printf ("Results of screenshot2d_test:\n");

  try
  {
      //настройка протоколирования
    
    common::LogFilter log_filter ("render.mid_level.Debug", &print);
    
    SceneRender render ("Debug", "Renderer2d", "Render2d");    
    
    RenderTarget render_target = render.RenderTarget (0);

    render_target.CaptureImage ("data/screenshot.tga");       
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
