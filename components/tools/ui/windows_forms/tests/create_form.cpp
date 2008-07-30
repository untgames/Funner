#include "shared.h"

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
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
