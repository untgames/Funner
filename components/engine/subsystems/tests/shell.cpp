#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <common/log.h>

#include <engine/subsystem_manager.h>

using namespace engine;

const char* CONFIGURATION_FILE_NAME = "config_shell.xml";

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

int main ()
{
  printf ("Results of shell_test:\n");

  try  
  {            
    common::LogFilter log_filter ("engine.*", &log_handler);

    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
