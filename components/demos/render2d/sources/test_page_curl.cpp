#include "shared.h"

struct Test
{
  TestApplication      application;
  PageCurl::Pointer    curl;
  Sprite::Pointer      sprite;
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;

  Test ()
  {
      //создание сцены

    curl = PageCurl::Create ();

    curl->SetPageMaterial (PageCurlPageType_FrontLeft,  "sprite_material");
    curl->SetPageMaterial (PageCurlPageType_FrontRight, "sprite_material");
    curl->SetPageMaterial (PageCurlPageType_BackLeft,   "sprite_material");
    curl->SetPageMaterial (PageCurlPageType_BackRight,  "sprite_material");

    curl->BindToScene (scene);

    sprite = Sprite::Create ();

    sprite->SetMaterial ("sprite_material");
    sprite->SetColor    (1, 1, 1, 0.95f);
    sprite->SetPosition (0, 0, 0);
    sprite->SetScale    (0.5, 0.5, 1);
    sprite->BindToScene (scene, NodeBindMode_AddRef);

    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, 0);
    camera->SetLeft     (-1);
    camera->SetRight    (1);
    camera->SetTop      (1);
    camera->SetBottom   (-1);
    camera->SetZNear    (-2);
    camera->SetZFar     (2);

      //создание областей вывода

    Viewport vp;

    screen.SetBackgroundColor (1, 0, 0, 0);
    screen.SetBackgroundState (false);

    vp.SetName            ("Viewport1");
    vp.SetRenderPath      ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (0, 0, 0, 0);
    vp.EnableBackground   ();

    vp.SetArea (0, 0, 100, 100);

    screen.Attach (vp);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //установка idle-функции

    application.SetIdleHandler (xtl::bind (&Test::Idle, this));
  }

    //обработчик главного цикла приложени€
  void Idle ()
  {
    try
    {
      application.PostRedraw ();
    }
    catch (std::exception& exception)
    {
      printf ("exception at idle: %s\n", exception.what ());
    }
  }
};

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of test_sprites:\n");

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
