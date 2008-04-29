#include "shared.h"

void keyboard (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("window '%s': ", window.Title ());

  switch (event)
  {
    case WindowEvent_OnKeyDown: printf ("key '%s' scan '%s' down\n", get_key_name (context.key), get_key_scan_name (context.key_scan_code)); break;
    case WindowEvent_OnKeyUp:   printf ("key '%s' scan '%s' up\n", get_key_name (context.key), get_key_scan_name (context.key_scan_code)); break;
  }
}

void destroy (Window& window, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void print (const char* message)
{
  printf ("window debug: %s\n", message);
}

int main ()
{
  printf ("Results of keys1_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetTitle ("Test window");
    window.Show ();

    auto_connection connection1 = window.RegisterEventHandler (WindowEvent_OnKeyDown, &keyboard),
                    connection2 = window.RegisterEventHandler (WindowEvent_OnKeyUp, &keyboard),
                    connection3 = window.RegisterEventHandler (WindowEvent_OnDestroy, &destroy);

    window.SetDebugLog (&print);        

    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
