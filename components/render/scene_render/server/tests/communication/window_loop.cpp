#include "shared.h"

void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

void on_window_close ()
{
  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of window_loop_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);

    common::PropertyMap properties;
    
    properties.SetProperty ("ColorBits", 24);
    properties.SetProperty ("DepthBits", 16);

    window.RegisterEventHandler (syslib::WindowEvent_OnClose, xtl::bind (&on_window_close));
    window.Show ();

    server.AttachWindow ("my_window", window, properties);

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
