#include <cstdio>
#include <exception>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/ref.h>

#include <common/log.h>
#include <common/time.h>

#include <syslib/application.h>

#include <input/cursor.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace input;

const char* CONFIGURATION_FILE_NAME = "config_cursor.xml";

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

void idle (Cursor& cursor)
{
  static size_t last = common::milliseconds ();
  
  if (common::milliseconds () - last < 1000)
    return;
    
  cursor.SetPosition (0.5f, 0.5f);

  last = common::milliseconds ();    
}

int main ()
{
  printf ("Results of cursor_test:\n");

  try  
  {            
    common::LogFilter log_filter ("*", &log_handler);

    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);    
    
    Cursor cursor;

    AttachmentRegistry::Register ("MyCursor", cursor);
    
    cursor.SetImage ("data/aero_busy.ani");
    
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&idle, xtl::ref (cursor)));

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return syslib::Application::GetExitCode ();
}
