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
    curl->SetCurlPoint          (PageCurlCorner_RightBottom);
    curl->SetCurlRadius         (0.1f);
    curl->SetPageMaterial       (PageCurlPageType_BackLeft,   "page1");
    curl->SetPageMaterial       (PageCurlPageType_BackRight,  "page2");
    curl->SetPageMaterial       (PageCurlPageType_FrontLeft,  "page3");
    curl->SetPageMaterial       (PageCurlPageType_FrontRight, "page4");
    curl->SetCornerShadowOffset (0.1f);
    curl->SetCurlPointPosition  (0.1f, 0.1f);

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
    vp.SetTechnique       ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (1, 1, 1, 1);
    vp.EnableBackground   ();

    vp.SetArea (0, 0, 100, 100);
//    vp.SetArea (10, 10, 80, 80);

    screen.Attach (vp);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //загрузка ресурсов

    application.LoadResources ();

      //установка idle-функции

    application.SetIdleHandler (xtl::bind (&Test::Idle, this));

    application.Window ().RegisterEventHandler (syslib::WindowEvent_OnMouseMove, xtl::bind (&Test::OnMouseMove, this, _1, _3));
//    application.Window ().SetViewportHandler (xtl::bind (&Test::UpdateViewport, this, _1, _2));
//    application.Window ().InvalidateViewport ();
  }

  void UpdateViewport (syslib::Window& window, syslib::Rect& viewport)
  {
    size_t window_width  = window.ClientWidth (),
           window_height = window.ClientHeight ();

    double window_aspect_ratio = double (window_width) / double (window_height);

    size_t viewport_width = 0, viewport_height = 0;

    float aspect_ratio = 1.f;

    if (window_aspect_ratio / aspect_ratio < 1.0)
    {
        //полосы по вертикали

      viewport_width  = window_width;
      viewport_height = size_t (viewport_width / aspect_ratio);
    }
    else
    {
        //полосы по горизонтали

      viewport_height = window_height;
      viewport_width  = size_t (viewport_height * aspect_ratio);
    }

    viewport.left   = (window_width - viewport_width) / 2;
    viewport.top    = (window_height - viewport_height) / 2;
    viewport.right  = viewport.left + viewport_width;
    viewport.bottom = viewport.top + viewport_height;
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
    syslib::Rect viewport = window.Viewport ();

    float width  = (float)(viewport.right - viewport.left),
          height = (float)(viewport.bottom - viewport.top);

    math::vec3f corner_position = math::vec3f (CAMERA_LEFT + (context.cursor_position.x - viewport.left) / width * (CAMERA_RIGHT - CAMERA_LEFT), CAMERA_BOTTOM + (1 - (context.cursor_position.y - viewport.top) / height) * (CAMERA_TOP - CAMERA_BOTTOM), 0) * math::inverse (curl->WorldTM ());

   corner_position.x += curl->Size ().x / 2;
   corner_position.y += curl->Size ().y / 2;

    curl->SetCurlPointPosition (corner_position.x, corner_position.y);

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
