#include "shared.h"

int main ()
{
  printf ("Results of create_form_test:\n");
  
  try
  {    
    Test test;
    
    test.main_window.ExecuteCommand ("Application.MainForm.Text = 'Hello world'");
    test.main_window.ExecuteCommand ("Application.LoadConfiguration ('data/conf/*.xform')");
    
    syslib::Application::Run ();
    
    return syslib::Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
