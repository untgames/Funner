#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;

void print (const char* message)
{
  printf ("window debug: %s\n", message);
  fflush (stdout);  
}

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

void on_close (Window& window, WindowEvent event, const WindowEventContext& context)
{
  Application::Exit (0);
}

void on_application_initialized ()
{
  //try to create window, which can't fit screen
  window_holder.reset (new Window (WindowStyle_Overlapped, 4000, 4000));

  Window& window = *window_holder;

  window.SetBackgroundState (true);

  window.SetTitle ("Test window");
  window.Show ();
  window.SetFocus ();

  window.SetPosition (1000, 1000);

  //Set size which can't fit screen
  window.SetSize (4000, 4000);

  //Set position which can't fit screen
  window.SetPosition (1000, 1000);

  window.RegisterEventHandler (WindowEvent_OnClose, &on_close);

  window.SetDebugLog (&print);
}

int main ()
{
  printf ("Results of window5_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_print);
    
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
