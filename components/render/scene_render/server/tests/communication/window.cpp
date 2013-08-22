#include "shared.h"

void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

int main ()
{
  printf ("Results of window_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);

    server.AttachWindow ("my_window", window, "");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
