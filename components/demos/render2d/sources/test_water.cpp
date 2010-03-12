#include "shared.h"

const size_t GRID_SIZE = 80;

float frand (float min_value=0.0f, float max_value=1.0f)
{
  return min_value + float (rand ()) / RAND_MAX * (max_value - min_value);
}

struct Test
{
  TestApplication      application;
  HeightMap::Pointer   height_map;
  Water::Pointer       water;
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;

  Test ()
  {
      //создание сцены

    height_map = HeightMap::Create ();
    
    height_map->SetCellsCount (GRID_SIZE, GRID_SIZE);

    height_map->SetMaterial ("water_material");
    height_map->BindToScene (scene);
    height_map->Translate   (0, 0, 15);
    height_map->Scale       (10.0f, 10.0f, 1.0f);
    height_map->SetVerticesColor (math::vec4f (1.0f, 1.0f, 1.0f, 1.0f));
//    height_map->SetVerticesNormal (normalize (math::vec3f (1.0f, 0, 1.0f)));
    
    water = Water::Create (*height_map);
    
    water->NodeOwnsController ();

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
    vp.SetRenderPath      ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (0.7f, 0.7f, 0.7f, 0.7f);
    vp.EnableBackground   ();
    
    vp.SetArea (5, 5, 90, 90);

    screen.Attach (vp);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //настройка запросов рендеринга

    application.Render ().SetMaxDrawDepth (3);

    application.Render ().RegisterQueryHandler ("test_query", xtl::bind (&Test::SetupDynamicRenderTarget, this, _1, _2));

      //установка idle-функции

    application.SetIdleHandler (xtl::bind (&Test::Idle, this));
  }

    //настройка динамического целевого буфера рендеринга
  void SetupDynamicRenderTarget (RenderTarget& render_target, const char*)
  {
    render_target.SetScreen (&screen);
  }

    //обработчик главного цикла приложени¤
  void Idle ()
  {
    try
    {
      static size_t last_update = 0;

      if (common::milliseconds () - last_update >= 10)
      {
        height_map->Update ((common::milliseconds () - last_update) / 1000.0f);
        
        water->PutStorm (math::vec3f (frand ()-0.5, frand ()-0.5, 0.0f), 0.1);
        
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
  printf ("Results of test_water:\n");

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
