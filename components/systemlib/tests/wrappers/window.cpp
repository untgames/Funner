#include "shared.h"

void keyboard (Window& window, WindowEvent event, const WindowEventContext& context)
{
  printf ("window '%s': ", window.Title ());

  switch (event)
  {
    case WindowEvent_OnKeyDown: printf ("key %d down\n", context.key); break;
    case WindowEvent_OnKeyUp:   printf ("key %d up\n", context.key); break;
  }
}

void mousemove (Window& window, WindowEvent, const WindowEventContext& context)
{
  printf ("window '%s': mouse move x=%u y=%u\n", window.Title (), context.cursor_position.x, context.cursor_position.y);
}

void destroy (Window& window, WindowEvent, const WindowEventContext&)
{
  printf ("window '%s': destroyed\n", window.Title ());  

  Application::Exit (0);

  throw std::bad_alloc (); //тестирования распространения исключений в обработчиках событий
}

void app_exit ()
{
  printf ("application exit\n");
}

void print (const char* message)
{
  printf ("window debug: %s\n", message);
}

int main ()
{
  printf ("Results of window_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetTitle ("Test window");

    auto_connection connection1 = window.RegisterEventHandler (WindowEvent_OnKeyDown, &keyboard),
                    connection2 = window.RegisterEventHandler (WindowEvent_OnKeyUp, &keyboard),
                    connection3 = window.RegisterEventHandler (WindowEvent_OnMouseMove, &mousemove),
                    connection4 = window.RegisterEventHandler (WindowEvent_OnDestroy, &destroy),
                    connection5 = Application::RegisterEventHandler (ApplicationEvent_OnExit, &app_exit);
                    
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
