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

const char* CONFIGURATION_FILE_NAME      = "config.xml";
const char* TRANSLATION_TABLE1_FILE_NAME = "data/translation_table1.keymap";

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

void render_log_print (const char* message)
{
  printf ("%s\n", message);
}

void input_event_handler (const char* event)
{
  if (!xtl::xstrcmp ("exit", event))
    syslib::Application::Exit (0);

  printf ("New input event: '%s'\n", event);
}

int main ()
{
  printf ("Results of subsystems1_test:\n");

  try  
  {            
    common::LogFilter log_filter ("*", &log_handler);

    SubsystemManager manager;

    manager.Start (CONFIGURATION_FILE_NAME);    

    render::Screen screen1, screen2;
    
    screen1.SetBackgroundColor (0.8f, 0.f, 0.f, 0.f);
    screen2.SetBackgroundColor (0.f, 0.8f, 0.f, 0.f);

    AttachmentRegistry::Register ("Screen1", screen1);
    AttachmentRegistry::Register ("Screen2", screen2);
    AttachmentRegistry::Register ("game1", xtl::function<void (const char*)> (&input_event_handler));

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
