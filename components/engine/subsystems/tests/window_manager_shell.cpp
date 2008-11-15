#include <cstdio>

#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/log.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <render/scene_render.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;

const char* CONFIGURATION_FILE_NAME      = "config_window_manager_shell.xml";
const char* TRANSLATION_TABLE1_FILE_NAME = "data/translation_table1.keymap";

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

int main ()
{
  printf ("Results of window_manager_test:\n");

  try  
  {            
    common::LogFilter log_filter ("*", &log_handler);

    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);    

    syslib::Application::Run ();
  }
  catch (xtl::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return syslib::Application::GetExitCode ();
}
