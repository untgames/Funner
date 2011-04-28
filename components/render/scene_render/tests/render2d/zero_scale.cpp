#include "shared.h"

int main ()
{
  printf ("Results of zero_scale_test:\n");

  try
  {
      //настройка протоколирования

    common::LogFilter log_filter ("render.mid_level.Debug", &print);

      //создание сцены

    Sprite::Pointer      sprite1 = Sprite::Create (), sprite2 = Sprite::Create (), sprite3 = Sprite::Create ();
    TextLine::Pointer    text_line1 = TextLine::Create (), text_line2 = TextLine::Create ();
    OrthoCamera::Pointer camera = OrthoCamera::Create ();

    Scene scene;

    sprite1->BindToScene (scene, NodeBindMode_WeakRef);
    sprite2->BindToScene (scene, NodeBindMode_WeakRef);
    sprite3->BindToScene (scene, NodeBindMode_WeakRef);
    text_line1->BindToScene (scene, NodeBindMode_WeakRef);
    text_line2->BindToScene (scene, NodeBindMode_WeakRef);
    camera->BindToScene (scene, NodeBindMode_WeakRef);

    text_line1->SetText ("Sample");
    text_line1->SetFont ("data/times_new_roman.xfont");
    text_line2->SetText ("Sample");
    text_line2->SetFont ("data/times_new_roman.xfont");

    sprite1->SetScale (0.f, 1.f, 1.f);
    sprite2->SetScale (1.f, 0.0000001f, 1.f);
    text_line1->SetScale (1.f, 1.f, 0.f);

    sprite1->SetName ("Sprite1");
    sprite2->SetName ("Sprite2");
    sprite3->SetName ("Sprite3");
    camera->SetName ("Camera1");
    sprite1->SetMaterial ("sprite_material");
    sprite2->SetMaterial ("sprite_material");
    sprite3->SetMaterial ("sprite_material");

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
