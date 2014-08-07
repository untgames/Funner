#include "shared.h"

struct Test
{
  TestApplication            application;
  Sprite::Pointer            sprite [3];
  InputZone::Pointer         input_zone [3];
  PerspectiveCamera::Pointer camera;
  Scene                      scene;
  Screen                     screen;

  Test ()
  {
      //создание сцены

    sprite [0] = Sprite::Create ();
    
    sprite [0]->SetMaterial ("sprite_material");
    sprite [0]->SetScale (1.0f, 1.0f, 1.0f);
    sprite [0]->SetPosition (-3.0f, -3.0f, 2.5f);
    sprite [0]->Rotate (math::anglef (), math::anglef (), math::degree (22.5f));
    sprite [0]->SetColor (1.0f, 1.0f, 0.0f);
    sprite [0]->SetMode (SpriteMode_Oriented);
    
    sprite [0]->BindToScene (scene);
    
    sprite [1] = Sprite::Create ();
    
    sprite [1]->SetMaterial ("sprite_material");
    sprite [1]->SetScale (5.0f, 5.0f, 1.0f);
    sprite [1]->SetPosition (3.0f, 3.0f, 0.5f);
    sprite [1]->SetColor (1.0f, 0.0f, 1.0f);
    sprite [1]->SetMode (SpriteMode_Oriented);
    
    sprite [1]->BindToScene (scene);
    
    sprite [2] = Sprite::Create ();    

    sprite [2]->SetMaterial ("sprite_material");
    sprite [2]->SetScale (0.2f, 0.2f, 0.2f);
    sprite [2]->SetPosition (0.0f, -0.5f, -15.f);
    sprite [2]->SetColor (0.0f, 1.0f, 1.0f);
    sprite [2]->SetMode (SpriteMode_Oriented);
    
    sprite [2]->BindToScene (scene);        

    input_zone [0] = InputZone::Create ();

    input_zone [0]->SetName     ("zone1");
    input_zone [0]->SetPosition (sprite [0]->Position ());
    input_zone [0]->BindToScene (scene);
    
    input_zone [0]->RegisterNotificationHandler (&Test::InputNotify);
    
    input_zone [1] = InputZone::Create ();
    
    input_zone [1]->SetName     ("zone2");
    input_zone [1]->SetPosition (sprite [1]->Position ());
    input_zone [1]->BindToScene (scene);
    
    input_zone [1]->RegisterNotificationHandler (&Test::InputNotify);    
    
    input_zone [2] = InputZone::Create ();
    
    input_zone [2]->SetName     ("zone3");
    input_zone [2]->SetPosition (sprite [2]->Position ());
    input_zone [2]->BindToScene (scene);
    
    input_zone [2]->RegisterNotificationHandler (&Test::InputNotify);

    camera = PerspectiveCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, -20);
    camera->SetFovX     (math::degree (40.f));
    camera->SetFovY     (math::degree (40.f));
    camera->SetZNear    (1.f);
    camera->SetZFar     (1000.f);
    
      //создание областей вывода

    Viewport vp1;

    vp1.SetName            ("Viewport1");
    vp1.SetZOrder          (0);
    vp1.SetTechnique       ("default");
    vp1.SetCamera          (camera.get ());
    vp1.SetDepthRange      (0, 0.5f);
    
    vp1.SetArea (5, 5, 90, 90);

    screen.Attach (vp1);

      //настройка ввода
    
    application.InputManager ().SetScreen (&screen);
//    application.InputManager ().SetTouchSize (1.0f);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //загрузка ресурсов

    application.LoadResources ();
  }

  static void InputNotify (InputZoneModel& zone, const Viewport& vp, const InputZoneNotification notification, const InputZoneNotificationContext& context)
  {
    switch (notification)
    {
      case InputZoneNotification_OnTouchMove:
      case InputZoneNotification_OnPressMove:
      case InputZoneNotification_OnScreenTouchMove:
      case InputZoneNotification_OnScreenTouchUp:
      case InputZoneNotification_OnScreenTouchDown:
        return;
      default:
        break;
    }
    
    printf ("%s: zone='%s', viewport='%s', touch=%d, button=%d, world_position=[%.3f, %.3f, %.3f] local_position=[%.3f, %.3f]\n", get_name (notification), zone.Name (), vp.Name (),
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
