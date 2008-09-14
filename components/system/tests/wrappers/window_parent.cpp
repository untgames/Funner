#include "shared.h"

void event_handler (Window& parent_window, Window& child_window)
{
  child_window.SetParentHandle (parent_window.Handle ());
}

int main ()
{
  printf ("Results of window_parent_test:\n");
  
  try
  {
    Window parent_window (WindowStyle_Overlapped),
           child_window  (WindowStyle_Overlapped);

    parent_window.SetTitle ("Parent");
    child_window.SetTitle ("Child");
    
    child_window.RegisterEventHandler (WindowEvent_OnLeftButtonUp, bind (&event_handler, ref (parent_window), ref (child_window)));

    parent_window.Show ();
    child_window.Show ();

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
