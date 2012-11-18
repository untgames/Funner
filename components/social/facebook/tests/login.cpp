#include "shared.h"

void log_print (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

int main ()
{
  printf ("Results of login_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_print);
    
    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
