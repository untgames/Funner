#include "shared.h"

void input_notify (InputZoneModel&, const Viewport&, const char* event, const char* params)
{
  printf ("%s: %s\n", event, params);
}

int main ()
{
  printf ("Results of scene_input1_test:\n");
  
  try
  {  
    Scene scene;
    
    OrthoCamera::Pointer camera = OrthoCamera::Create ();
    
    camera->SetLeft (0);
    camera->SetRight (100);
    camera->SetTop (0);
    camera->SetBottom (-100);
    camera->SetZNear (-1);
    camera->SetZFar (1);
    
    camera->Translate (0.0f, 0.0f, 1.0f);
    
    camera->BindToScene (scene);
    
    InputZone::Pointer zone = InputZone::Create ();
    
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
