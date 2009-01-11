#include "shared.h"

const char* APPLICATION_SERVER_REGISTRY_BRANCH = "ApplicationServer";

extern "C"
{

extern double _HUGE = DBL_MAX;

}

void print (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

void log_handler (const char* log_name, const char* log_message)
{
  common::Console::Printf ("%s: %s\n", log_name, log_message);
}

int main ()
{
  common::LogFilter log_filter ("*", &log_handler);

  printf ("Results of create_form_test:\n");

  try
  {
    common::VarRegistryContainer<stl::string> application_server_registry_container;

    application_server_registry_container.Mount (APPLICATION_SERVER_REGISTRY_BRANCH);

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
