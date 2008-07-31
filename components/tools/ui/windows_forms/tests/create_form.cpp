#include "shared.h"

#using <System.Windows.Forms.dll>

void print (const char* message)
{
  printf ("%s\n", message);
}

void idle ()  
{
  System::Windows::Forms::Application::DoEvents ();
}

int main ()
{
  printf ("Results of create_form_test:\n");
  
  try
  {    
    Test test;
    
    test.main_window.SetLogHandler (&print);
    test.main_window.ExecuteFile ("data/ui.lua");    
    
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, &idle);
    
    syslib::Application::Run ();
    
    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
