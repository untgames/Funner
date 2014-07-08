#include "shared.h"

void input_notify (InputZoneModel& model, const Viewport&, const InputZoneNotification notification, const InputZoneNotificationContext& context)
{
  switch (notification)
  {
    case InputZoneNotification_OnScreenTouchMove:
    case InputZoneNotification_OnScreenTouchUp:
    case InputZoneNotification_OnScreenTouchDown:
    case InputZoneNotification_OnTouchEnter:
    case InputZoneNotification_OnTouchLeave:
      return;
    default:
      break;
  }

  printf ("%s: event=%s, touch=%d, button=%d, world_position=[%.3f, %.3f, %.3f] local_position=[%.3f, %.3f]\n", model.Name (), get_name (notification), 
    context.touch_id, context.button, context.touch_world_position.x, context.touch_world_position.y, context.touch_world_position.z,
    context.touch_local_position.x, context.touch_local_position.y);
}

int main ()
{
  printf ("Results of scene_input_on_click_test:\n");
  
  try
  {  
    Scene scene;
    
    OrthoCamera::Pointer camera = OrthoCamera::Create ();
    
    camera->SetLeft (100);
    camera->SetRight (0);
    camera->SetTop (0);
    camera->SetBottom (-100);
    camera->SetZNear (-1);
    camera->SetZFar (1);
    
    camera->Translate (0.0f, 0.0f, 1.0f);
    
    camera->BindToScene (scene);
    
    InputZone::Pointer zone1 = InputZone::Create ();
    
    zone1->SetName ("zone1");
    
    zone1->Translate (25.0f, -50.0f, 0.0f);
    zone1->SetScale (50.0f, 100.0f, 1.0f);
    
    zone1->RegisterNotificationHandler (&input_notify);
    
    zone1->BindToScene (scene);
    
    InputZone::Pointer zone2 = InputZone::Create ();
    
    zone2->SetName ("zone2");
    
    zone2->Translate (75.0f, -50.0f, 0.0f);
    zone2->SetScale (50.0f, 100.0f, 1.0f);
    
    zone2->RegisterNotificationHandler (&input_notify);
    
    zone2->BindToScene (scene);    

    Screen screen;
    
    Viewport viewport;
    
    viewport.SetCamera (&*camera);
    viewport.SetArea (100, 100, 800, 800);
    
    screen.SetArea (0, 0, 1000, 1000);
    screen.Attach (viewport);
    
    SceneInputManager manager;

    manager.SetScreen (&screen);
    manager.SetTouchSize (2.0f, InputTransformSpace_Camera);

    manager.ProcessEvent ("CursorX axis -0.5");
    manager.ProcessEvent ("CursorY axis 0");
    manager.ProcessEvent ("Mouse0 down");
    manager.ProcessEvent ("CursorX axis 0.5");
    manager.ProcessEvent ("Mouse0 up");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
