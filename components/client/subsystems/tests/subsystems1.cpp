#include <cstdio>

#include <xtl/exception.h>
#include <xtl/function.h>

#include <common/log.h>
#include <common/strlib.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <render/scene_render.h>

#include <client/engine.h>
#include <client/window_manager.h>

using namespace client;

const char* CONFIGURATION_BRANCH_NAME = "Configuration";

void log_handler (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

void render_log_print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of subsystems1_test:\n");

  try  
  {            
    common::LogFilter log_filter ("*", &log_handler);

    Engine engine (CONFIGURATION_BRANCH_NAME);

    EngineAttachments client;

    render::Screen screen1, screen2;
    
    screen1.SetBackgroundColor (0.8f, 0.f, 0.f, 0.f);
    screen2.SetBackgroundColor (0.f, 0.8f, 0.f, 0.f);

    client.SetScreen ("Screen1", screen1);
    client.SetScreen ("Screen2", screen2);

    engine.Attach (client);

    syslib::Application::Run ();
  }
  catch (xtl::exception& e)
  {
    printf ("exception: %s\n", e.what ());

    return 1;
  }
  catch (...)
  {
    printf ("Unknown exception\n");

    return 1;
  }

  return 0;
}
