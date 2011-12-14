#include "shared.h"

int main ()
{
  printf ("Results of init2d_test:\n");

  try
  {
    SceneRender render ("Debug", "Renderer2d", "Render2d");    

    printf ("renderer: %s\n", render.RendererDescription ());
    printf ("paths:    %s\n", render.RenderPaths ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
