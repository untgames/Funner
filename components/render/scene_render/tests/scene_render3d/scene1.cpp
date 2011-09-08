#include "shared.h"

int main ()
{
  printf ("Results of scene1_test:\n");
  
  try
  {
    Test test (L"Scene1 test");
    
    RenderManager manager = test.RenderManager ();
    
    manager.LoadResource ("data/render.rfx");
    
    SceneRender render;
    
    render.SetRenderManager (&manager);
    
    render.SetRenderTarget ("default_color", test.Window ().ColorBuffer ());
    render.SetRenderTarget ("default_depth_stencil", test.Window ().DepthStencilBuffer ());

    scene_graph::Screen screen;        
    
    render.SetScreen (&screen);
    
    screen.SetBackgroundState (false);    
    
    scene_graph::OrthoCamera::Pointer camera = scene_graph::OrthoCamera::Create ();
    
      //настройка области вывода
    
    scene_graph::Viewport vp;

    vp.SetArea      (0, 0, 100, 100);
    vp.SetCamera    (&*camera);
    vp.SetTechnique ("default");
    
    screen.Attach (vp);
    
    render.Update ();

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
