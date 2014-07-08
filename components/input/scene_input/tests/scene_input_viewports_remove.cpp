#include "shared.h"

Screen* screen;

size_t first_viewport_id = 0;

void input_notify (InputZoneModel& model, const Viewport& vp, const InputZoneNotification notification, const InputZoneNotificationContext& context)
{
  printf ("%s: %s\n", vp.Name (), get_name (notification));
  fflush (stdout);  

  if (notification == InputZoneNotification_OnPress && vp.Id () == first_viewport_id)
    screen->Detach (screen->Viewport (screen->ViewportsCount () - 1));
    
  if (notification == InputZoneNotification_OnTouchEnter && vp.Id () == first_viewport_id && context.button == -1)
  {
    Viewport viewport;
    
    viewport.SetCamera (const_cast<Camera*> (vp.Camera ()));
    viewport.SetArea (vp.Area ());
    viewport.SetName ("viewport2");
    
    screen->Attach (viewport);
  }
}

int main ()
{
  printf ("Results of scene_input_viewports_remove_test:\n");
  
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
    
    zone->Translate (50.0f, -50.0f, 0.0f);
    zone->SetScale (10.0f, 10.0f, 1.0f);
    
    zone->RegisterNotificationHandler (&input_notify);    
    
    zone->BindToScene (scene);    
    
    Screen screen;
    
    ::screen = &screen;
    
    Viewport viewport;
    
    viewport.SetName ("viewport1");
    
    viewport.SetCamera (&*camera);
    viewport.SetArea (100, 100, 800, 800);    
    
    screen.SetArea (0, 0, 1000, 1000);
    screen.Attach (viewport);
    
    first_viewport_id = viewport.Id ();
    
    SceneInputManager manager;
    
    manager.SetScreen (&screen);
    manager.SetTouchSize (2.0f, InputTransformSpace_Camera);

    manager.ProcessEvent ("CursorX axis 0");
    manager.ProcessEvent ("CursorY axis 0");
    manager.ProcessEvent ("Mouse0 down");
    
    printf ("exit\n");
    fflush (stdout);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
