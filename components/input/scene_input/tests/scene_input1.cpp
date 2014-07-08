#include "shared.h"

void input_notify (InputZoneModel&, const Viewport&, const InputZoneNotification notification, const InputZoneNotificationContext& context)
{
  printf ("%s: touch=%d, button=%d, world_position=[%.3f, %.3f, %.3f] local_position=[%.3f, %.3f]\n", get_name (notification), 
    context.touch_id, context.button, context.touch_world_position.x, context.touch_world_position.y, context.touch_world_position.z,
    context.touch_local_position.x, context.touch_local_position.y);
}

int main ()
{
  printf ("Results of scene_input1_test:\n");
  
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
    
    InputZone::Pointer zone = InputZone::Create ();
    
    zone->SetName ("zone1");
    
    zone->Translate (50.0f, -50.0f, 0.0f);
    zone->SetScale (10.0f, 10.0f, 1.0f);
    
    zone->RegisterNotificationHandler (&input_notify);
    
    zone->BindToScene (scene);
    
    Screen screen;
    
    Viewport viewport;
    
    viewport.SetCamera (&*camera);
    viewport.SetArea (100, 100, 800, 800);
    
    screen.SetArea (0, 0, 1000, 1000);
    screen.Attach (viewport);
    
    SceneInputManager manager;

    manager.SetScreen (&screen);
    manager.SetTouchSize (2.0f, InputTransformSpace_Camera);

    manager.ProcessEvent ("CursorX axis 0");
    manager.ProcessEvent ("CursorY axis 0");
    manager.ProcessEvent ("Mouse0 down");
    manager.ProcessEvent ("Mouse0 up");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
