#include "shared.h"

extern "C"
{

extern double _HUGE = DBL_MAX;

}

void print (const char* message)
{
  printf ("%s\n", message);
}

void log_handler (const char* log_name, const char* log_message)
{
  printf ("%s: %s\n", log_name, log_message);
}

int main ()
{
  common::LogFilter log_filter ("*", &log_handler);

  printf ("Results of create_form_test:\n");  
  
  try
  {    
    Test test;
    
    test.main_window.SetLogHandler (&print);
    test.main_window.ExecuteFile ("data/ui.lua");        

    syslib::Application::Run ();
    
    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
