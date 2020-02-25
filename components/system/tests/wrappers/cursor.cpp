#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window1_holder;
WindowPtr window2_holder;

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void on_application_initialized ()
{
  try
  {
    window1_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));
    window2_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

    Window& window1 = *window1_holder;
    Window& window2 = *window2_holder;

    window2.SetCursor ("aero_busy.ani");

    window1.HideCursor ();
    window1.Show ();
    window2.Show ();

    window1.RegisterEventHandler (WindowEvent_OnClose, &destroy);
    window2.RegisterEventHandler (WindowEvent_OnClose, &destroy);

    Point cursor_position = window1.CursorPosition ();

    printf ("Cursor position before = %ux%u\n", cursor_position.x, cursor_position.y);

    window1.SetCursorPosition (200, 200);

    cursor_position = window1.CursorPosition ();

    printf ("Cursor position after = %ux%u\n", cursor_position.x, cursor_position.y);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception '%s'\n", e.what ());
    Application::Exit (0);
  }
  catch (...)
  {
    printf ("Unknown exception\n");
    Application::Exit (0);
  }
}

int main ()
{
  printf ("Results of cursor_test:\n");
  
  try
  {
    common::FileSystem::AddSearchPath ("data");

    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
