#include <cstdio>

#include <stl/string>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/lexical_cast.h>

#include <common/log.h>
#include <common/strlib.h>

#include <syslib/application.h>

#include <client/window_manager.h>

const char* CONFIGURATION_BRANCH_NAME = "Configuration";

using namespace client;

void log_handler (const char* log_name, const char* message)
{
  printf ("Log '%s' message: '%s'\n", log_name, message);
}

void window_startup_handler (syslib::Window& window, common::VarRegistry& var_registry, Engine& engine)
{
  printf ("Starting window with title '%s', width %u, height %u, position %u %u\n", window.Title (), window.Width (), window.Height (), 
                                                                                    window.Position ().x, window.Position ().y);

  if (!var_registry.HasVariable ("StartupParam"))
    throw xtl::format_operation_exception ("window_startup_handler", "Can't start window, no 'startup_param' property");

  printf ("Var registry startup param is '%s'\n", var_registry.GetValue ("StartupParam").cast<stl::string> ().c_str ());
}

int main ()
{
  printf ("Results of window_manager1_test:\n");

  try
  {
    common::LogFilter log_filter ("*", &log_handler);

    Engine engine (CONFIGURATION_BRANCH_NAME);

    printf ("Engine configuration branch name is '%s'\n", engine.ConfigurationBranch ());
    printf ("Engine subsystems count is %u\n", engine.SubsystemsCount ());

    WindowManager::RegisterStartupHandler ("WindowStartup", &window_startup_handler);

    syslib::Window window1 (syslib::WindowStyle_Overlapped, 300, 300);

    window1.SetTitle ("window1");
    window1.Show ();

    common::VarRegistry var_registry (common::format ("%s.Window1", CONFIGURATION_BRANCH_NAME).c_str ());

    WindowManager::InitWindow (window1, var_registry, engine);

    var_registry.Open (common::format ("%s.Windows", CONFIGURATION_BRANCH_NAME).c_str ());

    WindowManager::InitWindows (var_registry, engine);

    printf ("Engine subsystems count is %u\n", engine.SubsystemsCount ());
    printf ("engine subsystem 0 name is '%s'\n", engine.Subsystem (0).Name ());

    syslib::Application::Run ();
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: '%s'\n", e.what ());

    return 1;
  }
  catch (...)
  {
    printf ("Unknown exception\n");

    return 1;
  }

  return 0;
}
