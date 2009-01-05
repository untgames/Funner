#include <cfloat>

#include <common/log.h>

#include <tools/ui/main_window.h>

#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

extern "C"
{

extern double _HUGE = DBL_MAX;

}

namespace
{

//класс тестового приложения
struct Test
{
  Test () :
    log ("modeler.test"),
    application_server (new MyApplicationServer, false),
    window_title_update_timer (xtl::bind (&Test::UpdateWindowTitle, this), 1000, syslib::TimerState_Paused)
  {
      //регистрация сервера приложения

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer", application_server.get ());

      //создание главного окна

    tools::ui::MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);
    window_title_update_timer.Run ();

      //подписка на события протоколирования

    main_window.SetLogHandler (xtl::bind (&Test::LogPrint, this, _1));

      //Открытие папки

    application_server->OpenProjectPath ("projects\\project1\\");
  }

  void LogPrint (const char* message)
  {
    log.Print (message);
  }

  void UpdateWindowTitle ()
  {
    if (application_server->CalculatingTrajectoriesCount ())
    {
      stl::string window_title = common::format ("Application.MainForm.Text = \"Modeler: calculating %u trajectories\"", application_server->CalculatingTrajectoriesCount ());

      main_window.ExecuteCommand (window_title.c_str ());
    }
    else
      main_window.ExecuteCommand ("Application.MainForm.Text = \"Modeler\"");
  }

  typedef xtl::com_ptr<MyApplicationServer> MyApplicationServerPtr;

  common::Log            log;
  MyApplicationServerPtr application_server;
  tools::ui::MainWindow  main_window;
  syslib::Timer          window_title_update_timer;            //таймер обновления заголовка окна
};

void print (const char* message)
{
  common::Console::Printf ("%s\n", message);
}

void log_handler (const char* log_name, const char* message)
{
  common::Console::Printf ("%s: %s\n", log_name, message);
}

}

int main ()
{
  try
  {
    common::LogFilter log_filter ("*", &log_handler);

    Test test;

    test.main_window.SetLogHandler (&print);
    test.main_window.ExecuteFile ("media/ui.lua");

    syslib::Application::Run ();

    return syslib::Application::GetExitCode ();
  }
  catch (xtl::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
