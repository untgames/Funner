#include "shared.h"

Screen* dynamic_screen_ptr = 0;

void setup_render_target (RenderTarget& render_target, const char* query_string)
{
  printf ("Create dynamic render target. Query='%s'\n", query_string);
  
  render_target.SetScreen (dynamic_screen_ptr);
}

int main ()
{
  printf ("Results of dynamic_texture_test:\n");

  try
  {
      //настройка протоколирования
    
    xtl::auto_connection log_connection = common::LogSystem::RegisterLogHandler ("render.mid_level.Debug", &print);
    
      //создание сцены

    Sprite::Pointer      sprite = Sprite::Create ();
    OrthoCamera::Pointer camera = OrthoCamera::Create ();

    Scene scene;

    sprite->BindToScene (scene, NodeBindMode_Capture);
    camera->BindToScene (scene, NodeBindMode_Capture);
    
    sprite->SetName ("Sprite1");
    camera->SetName ("Camera1");
    sprite->SetMaterial ("dynamic_material");

      //создание рендера

    SceneRender render ("Debug", "Renderer2d", "Render2d");            
    
    render.SetLogHandler (&log_print);

      //настройка запросов рендеринга

    render.RegisterQueryHandler ("test_*", &setup_render_target);

      //настройка областей вывода

    Viewport vp1;

    vp1.SetArea       (0, 0, 100, 100);
    vp1.SetCamera     (&*camera);
    vp1.SetRenderPath ("Render2d");

    Screen screen;
    
    dynamic_screen_ptr = &screen;        
    
    render.SetMaxDrawDepth (3);

    screen.Attach (vp1);        
    
    RenderTarget render_target;

    render_target = render.RenderTarget (0);

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
