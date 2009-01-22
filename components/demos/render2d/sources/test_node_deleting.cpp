#include "shared.h"

const size_t SPRITES_COUNT   = 10;
const char*  SPRITE_MATERIAL = "sprite_material";

typedef stl::queue <Sprite::Pointer> SpritesQueue;

struct Test
{
  TestApplication      application;
  SpritesQueue         sprites_queue;
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;
  float                current_angle;
  float                sprite_position_radius;
  float                angle_delta;

  Test ()
    : current_angle (0), sprite_position_radius (8), angle_delta (3.14f / 10.f)
  {
      //создание сцены

    for (size_t i = 0; i < SPRITES_COUNT; i++)
      CreateSprite ();

    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, -3);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (-20);
    camera->SetZFar     (20);

      //создание областей вывода

    Viewport vp;

    screen.SetBackgroundColor (0, 0, 0, 0);
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

  void CreateSprite ()
  {
    Sprite::Pointer sprite = Sprite::Create ();

    sprite->SetPosition (cos (current_angle) * sprite_position_radius, sin (current_angle) * sprite_position_radius, 0.f);
    sprite->SetColor    (1.f, 1.f, 1.f, 0.7f);
    sprite->SetMaterial (SPRITE_MATERIAL);
    sprite->BindToScene (scene);

    sprites_queue.push (sprite);

    printf ("Created sprite %p\n", sprite.get ());

    current_angle += angle_delta;
  }

    //обработчик главного цикла приложени€
  void Idle ()
  {
    try
    {
      static size_t last_update = 0;

      if (common::milliseconds () - last_update >= 200)
      {
        printf ("Removing sprite %p\n", sprites_queue.front ().get ());

        sprites_queue.pop ();
        CreateSprite ();

        last_update = common::milliseconds ();
      }

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
  printf ("Results of test_node_deleting:\n");

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
