#include "shared.h"

struct Test
{
  TestApplication      application;
  Sprite::Pointer      sprite [2];
  InputZone::Pointer   input_zone [2];
  OrthoCamera::Pointer camera;
  Scene                scene;
  Screen               screen;

  Test ()
  {
      //создание сцены

    sprite [0] = Sprite::Create ();
    
    sprite [0]->SetMaterial ("sprite_material");
    sprite [0]->SetScale (10.0f, 10.0f, 1.0f);        
    sprite [0]->SetPosition (-3.0f, -3.0f, 0.5f);                
    
    sprite [0]->BindToScene (scene);
    
    sprite [1] = Sprite::Create ();
    
    sprite [1]->SetMaterial ("sprite_material");
    sprite [1]->SetScale (10.0f, 10.0f, 1.0f);        
    sprite [1]->SetPosition (3.0f, 3.0f, 0.0f);            
    
    sprite [1]->BindToScene (scene);    
    
    input_zone [0] = InputZone::Create ();

    input_zone [0]->SetName      ("zone1");
    input_zone [0]->BindToParent (*sprite [0]);
    
    input_zone [0]->RegisterNotificationHandler (&Test::InputNotify);
    
    input_zone [1] = InputZone::Create ();
    
    input_zone [1]->SetName      ("zone2");    
    input_zone [1]->BindToParent (*sprite [1]);
    
    input_zone [1]->RegisterNotificationHandler (&Test::InputNotify);    

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
    
      //настройка ввода
    
    application.InputManager ().SetScreen (&screen);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //загрузка ресурсов

    application.LoadResources ();
  }

  static void InputNotify (InputZoneModel& zone, const Viewport&, const InputZoneNotification notification, const InputZoneNotificationContext& context)
  {
    switch (notification)
    {
      case InputZoneNotification_OnTouchMove:
      case InputZoneNotification_OnScreenTouchMove:
      case InputZoneNotification_OnScreenTouchUp:
      case InputZoneNotification_OnScreenTouchDown:
        return;
      default:
        break;
    }
    
    printf ("%s: zone='%s', touch=%d, button=%d, world_position=[%.3f, %.3f, %.3f] local_position=[%.3f, %.3f]\n", get_name (notification), zone.Name (),
      context.touch_id, context.button, context.touch_world_position.x, context.touch_world_position.y, context.touch_world_position.z,
      context.touch_local_position.x, context.touch_local_position.y);
      
    fflush (stdout);
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
