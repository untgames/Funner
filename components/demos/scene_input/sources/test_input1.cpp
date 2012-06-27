#include "shared.h"

struct Test
{
  TestApplication      application;
  Sprite::Pointer      sprite;
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;

  Test ()
  {
      //создание сцены

    sprite = Sprite::Create ();
    
    sprite->SetMaterial ("sprite_material");
    sprite->SetScale (15.0f, 15.0f, 1.0f);    
    
    sprite->SetProperties (common::PropertyMap ());
    
    sprite->BindToScene (scene);

    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, 0);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (0);
    camera->SetZFar     (20);

      //создание областей вывода

    Viewport vp;

    screen.SetBackgroundColor (1, 0, 0, 0);
    screen.SetBackgroundState (false);

    vp.SetName            ("Viewport1");
    vp.SetTechnique       ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (0.7f, 0.7f, 0.7f, 0.7f);
    vp.EnableBackground   ();
    
    vp.SetArea (5, 5, 90, 90);

    screen.Attach (vp);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //загрузка ресурсов

    application.LoadResources ();
  }

};

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of test_input1:\n");

  try
  {
      //настройка протоколирования

    common::LogFilter filter ("*", &log_print);

      //запуск теста

    Test test;

    return test.application.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
