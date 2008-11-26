#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/string.h>

#include <common/console.h>

#include <syslib/application.h>

#include <engine/subsystem_manager.h>

using namespace engine;

namespace
{

#ifndef LAUNCHER_VERSION
  #define LAUNCHER_VERSION "unknown version"
#endif

/*
     онстанты
*/

const char* VERSION                         = LAUNCHER_VERSION; //строка версии
const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";       //им€ конфигурационного файла по умолчанию
const bool  DEFAULT_HAS_MAIN_LOOP           = true;               //наличие главного цикла приложени€ по умолчанию

const char* KEY_CONFIGURATION = "--config=";      //им€ ключа конфигурационного файла
const char* KEY_NO_MAIN_LOOP  = "--no-main-loop"; //отключение главного цикла приложени€
const char* KEY_MAIN_LOOP     = "--main-loop";    //включение главного цикла приложени€
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


/*
    ѕриложение
*/

class Application
{
  public:
/// онструктор
    Application ()
    {
      has_main_loop      = DEFAULT_HAS_MAIN_LOOP;
      configuration_name = DEFAULT_CONFIGURATION_FILE_NAME;
      need_print_version = false;
      need_print_help    = false;
    }

///–азбор параметров командой строки
    void ParseCommandLine (size_t args_count, char* args [])
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

///¬ыполнение приложени€
    void Run ()
    {
      manager.Start (configuration_name.c_str ());

      if (need_print_version)
        common::Console::Printf ("Application version: %s\n", VERSION);

      if (need_print_help)
        for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
          common::Console::Print (HELP[i]);

        //запуск основного цикла приложени€

      if (has_main_loop)
      {
        syslib::Application::Run ();
      }
    }

  private:
    SubsystemManager manager;                    //менеджер подсистем
    bool             has_main_loop;              //есть ли главный цикл приложени€
    bool             has_explicit_configuration; //конфигураци€ приложени€ указана €вно
    stl::string      configuration_name;         //им€ конфигурации
    bool             need_print_version;         //нужно распечатать строку версии
    bool             need_print_help;            //нужно распечатать помощь по запуску приложени€
};

}

//точка входа
int main (int argc, char* argv [])
{
  try
  {
    Application application;

    application.ParseCommandLine ((size_t)argc, argv);

    application.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return syslib::Application::GetExitCode ();
}
