#include "shared.h"

int main ()
{
  printf ("Results of render2d_test:\n");

  try
  {
      //настройка протоколирования
    
    common::LogFilter log_filter ("render.mid_level.Debug", &print);
    
      //создание сцены

    Sprite::Pointer      sprite = Sprite::Create ();
    OrthoCamera::Pointer camera = OrthoCamera::Create ();

    Scene scene;

    sprite->BindToScene (scene, NodeBindMode_Capture);
    camera->BindToScene (scene, NodeBindMode_Capture);
    
    sprite->SetName ("Sprite1");
    camera->SetName ("Camera1");
    sprite->SetMaterial ("sprite_material");
//    sprite->SetFrame (20);

      //создание рендера

    SceneRender render ("Debug", "Renderer2d", "Render2d");            
    
    render.SetLogHandler (&log_print);

      //настройка областей вывода

    Viewport vp1;

    vp1.SetArea       (0, 0, 100, 100);
    vp1.SetCamera     (&*camera);
    vp1.SetRenderPath ("Render2d");

    Screen screen;

    screen.Attach (vp1);

    RenderTarget render_target = render.RenderTarget (0);

    render_target.SetScreen (&screen);

      //загрузка ресурсов
      
    render.LoadResource ("data/materials.xmtl");
    
      //отрисовка
      
    render_target.Update ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
