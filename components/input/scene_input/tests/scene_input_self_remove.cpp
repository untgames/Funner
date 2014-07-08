#include "shared.h"

InputZone::Pointer zone;
InputZone::Pointer additional_zone;

void input_notify (InputZoneModel& model, const Viewport&, const InputZoneNotification notification, const InputZoneNotificationContext& context)
{
  printf ("%s: %s\n", model.Name (), get_name (notification));
  fflush (stdout);  

  if (notification == InputZoneNotification_OnScreenTouchDown && &model != additional_zone && zone)
  {
    zone->Unbind ();
    
    zone = InputZone::Pointer ();    
  }
    
  if (notification == InputZoneNotification_OnScreenTouchUp && &model == additional_zone)
  {
    zone = InputZone::Create ();        
    
    zone->SetName ("zone3");
    
    zone->RegisterNotificationHandler (&input_notify);    
    
    zone->BindToScene (*model.Scene ());    
  }
}

int main ()
{
  printf ("Results of scene_input_self_remove_test:\n");
  
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
    
    zone = InputZone::Create ();        
    additional_zone = InputZone::Create ();        
    
    zone->SetName ("zone1");
    
    zone->Translate (50.0f, -50.0f, 0.0f);
    zone->SetScale (10.0f, 10.0f, 1.0f);
    
    zone->RegisterNotificationHandler (&input_notify);    
    
    zone->BindToScene (scene);
    
    additional_zone->SetName ("zone2");    
    
    additional_zone->RegisterNotificationHandler (&input_notify);    
    
    additional_zone->BindToScene (scene);    
    
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
    
    printf ("exit\n");
    fflush (stdout);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
