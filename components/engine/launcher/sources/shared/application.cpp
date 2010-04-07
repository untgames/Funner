#include <cstdio>
#include <exception>

#include <stl/auto_ptr.h>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/string.h>

#include <syslib/application.h>

#include <engine/subsystem_manager.h>

#include <engine/launcher.h>

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
  "Usage: app_name [flags] [source] ...\n",
  "  flags:\n",
  "    --config=config_file_name - launch with specified config file\n",
  "    --version (-v)            - print version\n",
  "    --no-main-loop            - app exits after starting subsystems\n",
  "    --main-loop               - app runs main loop after starting subsystems\n",
  "    --help                    - print this help\n",
};

const size_t STARTUP_MAIN_LOOP_DELAY = 10; //задержка перед запуском главной нити приложени€ (в милисекундах) - необходимо дл€ работы на iPhone

/*
    ѕриложение
*/

class Application: public IFunnerApi
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
    bool ParseCommandLine (unsigned int args_count, const char** args)
    {
      try
      {
        commands.Reserve (args_count);
        
        for (size_t i=1; i<args_count; i++)
        {
          const char* argument = args [i];

          if (!argument)
            continue;

          if (!xtl::xstrncmp (argument, KEY_CONFIGURATION, xtl::xstrlen (KEY_CONFIGURATION)))
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
            has_main_loop   = false;
          }
          else
          {
            commands.Add (argument);
          }
        }
        
        return true;
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at Application::ParseCommandLineEntry\n");
      }
      
      return false;
    }

///¬ыполнение приложени€
    void Run ()
    {
      try
      {
          //регистраци€ обработчика старта приложени€

        syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialized, xtl::bind (&Application::StartupHandler, this));

          //запуск основного цикла
  
        syslib::Application::Run ();
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
      }
      catch (...)
      {
        printf ("unknown exception at Application::RunEntry\n");
      }      
    }

  private:
///ќбработчик старта приложени€
    void StartupHandler ()
    {
      try
      {      
          //запуск подсистем

        if (need_print_version)
          common::Console::Printf ("Application version: %s\n", VERSION);

        if (need_print_help)
          for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
            common::Console::Print (HELP [i]);

          //если основного цикла нет - выход из приложени€

        if (!has_main_loop)
          syslib::Application::Exit (0);            

        if (!need_print_help && !need_print_version)
        {
          try
          {
            manager.Start (configuration_name.c_str ());
            
            for (size_t i=0, count=commands.Size (); i<count; i++)
            {
              const char* command = commands [i];

              manager.Execute (command);
            }
          }
          catch (...)
          {
            syslib::Application::Exit (-1);
            throw;
          }
        }
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
    SubsystemManager    manager;                    //менеджер подсистем
    bool                has_main_loop;              //есть ли главный цикл приложени€
    bool                has_explicit_configuration; //конфигураци€ приложени€ указана €вно
    stl::string         configuration_name;         //им€ конфигурации
    bool                need_print_version;         //нужно распечатать строку версии
    bool                need_print_help;            //нужно распечатать помощь по запуску приложени€
    common::StringArray commands;                   //команды на выполнение подсистемами
};

}

/*
    ќбЄртки над обращени€ми к приложению
*/

extern "C" int MAKE_TARGET_LINK_INCLUDES_COMMA; 

static int touch [] = {MAKE_TARGET_LINK_INCLUDES_COMMA};

FUNNER_C_API IFunnerApi* FunnerInit ()
{
  try
  {    
    return new Application;
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
