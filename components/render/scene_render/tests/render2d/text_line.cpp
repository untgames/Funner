#include "shared.h"

int main ()
{
  printf ("Results of text_line_test:\n");

  try
  {
      //настройка протоколирования
    
    xtl::auto_connection log_connection = common::LogSystem::RegisterLogHandler ("render.mid_level.Debug", &print);
    
      //создание сцены

    TextLine::Pointer    text_line = TextLine::Create ();
    OrthoCamera::Pointer camera    = OrthoCamera::Create ();

    Scene scene;

    text_line->BindToScene (scene, NodeBindMode_Capture);
    camera->BindToScene     (scene, NodeBindMode_Capture);
    
    text_line->SetName ("TextLine1");
    camera->SetName ("Camera1");
    text_line->SetColor (0.1f, 0.2f, 0.3f, 0.4f);
    text_line->SetText ("Text");
    text_line->SetFont ("data/times_new_roman.xfont");
    text_line->SetAlignment (TextLineAlignment_Center, TextLineAlignment_Center);

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
    
    RenderTarget render_target;

    render_target = render.RenderTarget (0);

    render_target.SetScreen (&screen);
    
      //отрисовка
      
    render_target.Update ();

    text_line->SetAlignment (TextLineAlignment_Right, TextLineAlignment_Top);
    
    render_target.Update ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
