#include <cstdio>
#include <exception>

#include <stl/auto_ptr.h>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/singleton.h>

#include <syslib/application.h>
#include <syslib/timer.h>

#include <engine/subsystem_manager.h>

#include <engine/launcher.h>

using namespace engine;

#ifdef _WIN32
  #define FUNNER_C_API __declspec(dllexport)
#elif __GNUC__
  #define FUNNER_C_API
#else
  #error "Unknown platform"
#endif

namespace
{

#ifndef LAUNCHER_VERSION
  #define LAUNCHER_VERSION "unknown version"
#endif

/*
    Константы
*/

const char* VERSION                         = LAUNCHER_VERSION; //строка версии
const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";       //имя конфигурационного файла по умолчанию
const bool  DEFAULT_HAS_MAIN_LOOP           = true;               //наличие главного цикла приложения по умолчанию

const char* KEY_CONFIGURATION = "--config=";      //имя ключа конфигурационного файла
const char* KEY_NO_MAIN_LOOP  = "--no-main-loop"; //отключение главного цикла приложения
const char* KEY_MAIN_LOOP     = "--main-loop";    //включение главного цикла приложения
const char* KEY_VERSION       = "--version";      //вывод справки в стандартный поток вывода
const char* KEY_VERSION_SHORT = "-v";             //вывод справки в стандартный поток вывода
const char* KEY_HELP          = "--help";         //вывод справки в стандартный поток вывода

const char* HELP [] = {
  "Usage: app_name [args]\n",
  "  args:\n",
  "    --config=config_file_name - launch with specified config file\n",
  "    --version (-v)            - print version\n",
  "    --no-main-loop            - app exits after starting subsystems\n",
  "    --main-loop               - app runs main loop after starting subsystems\n",
  "    --help                    - print this help\n",
};

const size_t STARTUP_MAIN_LOOP_DELAY = 10; //задержка перед запуском главной нити приложения (в милисекундах) - необходимо для работы на iPhone


/*
    Приложение
*/

class Application
{
  public:
///Конструктор
    Application ()
    {
      has_main_loop      = DEFAULT_HAS_MAIN_LOOP;
      configuration_name = DEFAULT_CONFIGURATION_FILE_NAME;
      need_print_version = false;
      need_print_help    = false;
    }

///Разбор параметров командой строки
    void ParseCommandLine (size_t args_count, const char* args [])
    {
      for (size_t i=0; i<args_count; i++)
      {
        const char* argument = args [i];

        if (!argument)
          continue;

        if (!xtl::xstrcmp (argument, KEY_CONFIGURATION))
        {
          configuration_name = argument + xtl::xstrlen (KEY_CONFIGURATION);
        }
        else if (!xtl::xstrcmp (argument, KEY_MAIN_LOOP))
        {
          has_main_loop = true;
        }
        else if (!xtl::xstrcmp (argument, KEY_NO_MAIN_LOOP))
        {
          has_main_loop = false;
        }
        else if (!xtl::xstrcmp (argument, KEY_VERSION) || !xtl::xstrcmp (argument, KEY_VERSION_SHORT))
        {
          need_print_version = true;
        }
        else if (!xtl::xstrcmp (argument, KEY_HELP))
        {
          need_print_help = true;
        }
      }
    }

///Выполнение приложения
    void Run ()
    {
        //регистрация обработчика старта приложения

      syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnEnterRunLoop, xtl::bind (&Application::StartupHandler, this));

        //запуск основного цикла

      syslib::Application::Run ();
    }

  private:
///Обработчик старта приложения
    void StartupHandler ()
    {
      try
      {
        startup_timer = new syslib::Timer (xtl::bind (&Application::StartupTimerHandler, this), STARTUP_MAIN_LOOP_DELAY);
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at engine::Application::StartupHandler\n");
      }

    }

///Обработчик событий таймера запуска приложения
    void StartupTimerHandler ()
    {
      try
      {
        startup_timer.reset ();

          //запуск подсистем

        manager.Start (configuration_name.c_str ());

        if (need_print_version)
          common::Console::Printf ("Application version: %s\n", VERSION);

        if (need_print_help)
          for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
            common::Console::Print (HELP [i]);

          //если основного цикла нет - выход из приложения

        if (!has_main_loop)
          syslib::Application::Exit (0);
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at engine::Application::StartupTimerHandler\n");
      }
    }

  private:
    SubsystemManager             manager;                    //менеджер подсистем
    bool                         has_main_loop;              //есть ли главный цикл приложения
    bool                         has_explicit_configuration; //конфигурация приложения указана явно
    stl::string                  configuration_name;         //имя конфигурации
    bool                         need_print_version;         //нужно распечатать строку версии
    bool                         need_print_help;            //нужно распечатать помощь по запуску приложения
    stl::auto_ptr<syslib::Timer> startup_timer;              //таймер необходимый для запуска приложения на встроенных платформах (пример - iPhone)
};

typedef common::Singleton<Application> ApplicationSingleton;

/*
    Точки входа
*/

struct FunnerApiImpl: public FunnerApi
{
  FunnerApiImpl ()
  {
    ParseCommandLine = &ParseCommandLineEntry;
    Run              = &RunEntry;
  }

  static bool ParseCommandLineEntry (unsigned int argc, const char** argv)
  {
    try
    {
      ApplicationSingleton::Instance ()->ParseCommandLine (argc, argv);

      return true;
    }
    catch (std::exception& exception)
    {
      printf ("exception: %s\n", exception.what ());
    }
    catch (...)
    {
      printf ("unknown exception at FunnerApplicationEntries::ParseCommandLineEntry\n");
    }

    return false;
  }

  static void RunEntry ()
  {
    try
    {
      ApplicationSingleton::Instance ()->Run ();
    }
    catch (std::exception& exception)
    {
      printf ("exception: %s\n", exception.what ());
    }
    catch (...)
    {
      printf ("unknown exception at FunnerApplicationEntries::RunEntry\n");
    }
  }
};

}

/*
    Обёртки над обращениями к приложению
*/

extern "C"
{

FUNNER_C_API FunnerApi* FunnerInit ()
{
  try
  {
    return new FunnerApiImpl;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception at FunnerInit\n");
  }

  return 0;
}

FUNNER_C_API void FunnerShutdown (FunnerApi* api)
{
  delete api;
}

}
