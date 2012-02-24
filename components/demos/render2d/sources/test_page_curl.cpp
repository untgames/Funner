#include "shared.h"

namespace
{

const float CAMERA_LEFT   = -1.1f;
const float CAMERA_RIGHT  = 1.1f;
const float CAMERA_TOP    = 0.9f;
const float CAMERA_BOTTOM = -0.9f;

}

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

    curl->SetSize               (2.f, 1.5f);
    curl->SetGridSize           (100, 100);
    curl->SetMode               (PageCurlMode_DoublePageDoubleMaterial);
    curl->SetCurlCorner         (PageCurlCorner_RightBottom);
    curl->SetCurlRadius         (0.1f);
    curl->SetPageMaterial       (PageCurlPageType_BackLeft,   "page1");
    curl->SetPageMaterial       (PageCurlPageType_BackRight,  "page2");
    curl->SetPageMaterial       (PageCurlPageType_FrontLeft,  "page3");
    curl->SetPageMaterial       (PageCurlPageType_FrontRight, "page4");
    curl->SetShadowMaterial     ("page_shadow");
    curl->SetCornerShadowOffset (0.1f);
    curl->SetPosition           (-1.f, -0.75f, 0.f);
    curl->SetCornerPosition     (0.1f, 0.1f);

    curl->BindToScene (scene);

/*    sprite = Sprite::Create ();

    sprite->SetMaterial ("sprite_material");
    sprite->SetColor    (1, 1, 1, 0.95f);
    sprite->SetPosition (0, 0, 0);
    sprite->SetScale    (0.5, 0.5, 1);
    sprite->BindToScene (scene, NodeBindMode_AddRef);*/

    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetLeft     (CAMERA_LEFT);
    camera->SetRight    (CAMERA_RIGHT);
    camera->SetTop      (CAMERA_TOP);
    camera->SetBottom   (CAMERA_BOTTOM);
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

    application.Window ().RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Test::OnMouseMove, this, _1, _3));
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

    //обработчик движени€ мыши
  void OnMouseMove (const syslib::Window& window, const syslib::WindowEventContext& context)
  {
    syslib::Rect client_rect = window.ClientRect ();

    float width  = (float)(client_rect.right - client_rect.left),
          height = (float)(client_rect.bottom - client_rect.top);

    curl->SetCornerPosition (CAMERA_LEFT + context.cursor_position.x / width * (CAMERA_RIGHT - CAMERA_LEFT) - curl->Position ().x, CAMERA_BOTTOM + (1 - context.cursor_position.y / height) * (CAMERA_TOP - CAMERA_BOTTOM) - curl->Position ().y);

//    printf ("Corner position = %f %f\n", curl->CornerPosition ().x, curl->CornerPosition ().y);
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
