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
    
    camera->SetLeft   (-1000.0f);
    camera->SetRight  (1000.0f);    
    camera->SetBottom (-1000.0f);
    camera->SetTop    (1000.0f);    
    camera->SetZNear  (-1000.0f);
    camera->SetZFar   (1000.0f);    
    
    scene_graph::Scene scene;
    
    camera->BindToScene (scene);
    
    scene_graph::VisualModel::Pointer model = scene_graph::VisualModel::Create ();
    scene_graph::PointLight::Pointer  light = scene_graph::PointLight::Create ();
    
    model->SetMeshName ("u1");
    
    light->BindToScene (scene);
    model->BindToScene (scene);    
    
      //настройка области вывода
    
    scene_graph::Viewport vp;
    
    common::PropertyMap properties;

    vp.SetArea       (0, 0, 100, 100);
    vp.SetCamera     (&*camera);
    vp.SetTechnique  ("default");
    vp.SetProperties (properties);    
    
    screen.Attach (vp);
    
    properties.SetProperty ("user_defined", "1.2");
    
    render.Update ();

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
