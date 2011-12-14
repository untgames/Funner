#include "shared.h"

using namespace engine;

namespace
{

#ifndef LAUNCHER_VERSION
  #define LAUNCHER_VERSION "unknown version"
#endif

/*
     онстанты
*/

const char* VERSION                         = LAUNCHER_VERSION;   //строка версии
const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";       //им€ конфигурационного файла по умолчанию
const bool  DEFAULT_HAS_MAIN_LOOP           = true;               //наличие главного цикла приложени€ по умолчанию
const char* STARTUP_ENV_VARS_FILE_NAME      = "/anonymous/system/startup_environment_variables.xml"; //им€ файла с переменными среды при запуске

const char* KEY_CONFIGURATION = "--config=";      //им€ ключа конфигурационного файла
const char* KEY_SEARCH_PATH   = "--search-path="; //путь к каталогу поиска файлов
const char* KEY_NO_MAIN_LOOP  = "--no-main-loop"; //отключение главного цикла приложени€
const char* KEY_MAIN_LOOP     = "--main-loop";    //включение главного цикла приложени€
const char* KEY_VERSION       = "--version";      //вывод справки в стандартный поток вывода
const char* KEY_VERSION_SHORT = "-v";             //вывод справки в стандартный поток вывода
const char* KEY_HELP          = "--help";         //вывод справки в стандартный поток вывода

const char* HELP [] = {
  "Usage: app_name [flags] [source] ...\n",
  "  flags:\n",
  "    --config=config_file_name - launch with specified config file\n",
  "    --search-path=dir_name    - add dir_name to search paths\n",
  "    --version (-v)            - print version\n",  
  "    --no-main-loop            - app exits after starting subsystems\n",
  "    --main-loop               - app runs main loop after starting subsystems\n",
  "    --help                    - print this help\n",
};

/*
    ѕриложение
*/

class Application: public IEngine
{
  public:
/// онструктор
    Application ()
      : environment_variables_file ((size_t)0, common::FileMode_ReadWrite | common::FileMode_Create)
    {
      has_main_loop      = DEFAULT_HAS_MAIN_LOOP;
      configuration_name = DEFAULT_CONFIGURATION_FILE_NAME;
      need_print_version = false;
      need_print_help    = false;
    }
    
    ///”становка базовой директории
    void SetBaseDir (const char* dir_name)
    {
      try
      {
        common::FileSystem::SetCurrentDir (dir_name);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Application::SetBaseDir");
        throw;
      }      
    }
    
    ///ѕолучение базовой директории
    const char* GetBaseDir ()
    {
      try
      {
        return common::FileSystem::GetCurrentDir ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Application::GetBaseDir");
        throw;
      }      
    }
        
///–азбор параметров командой строки
    bool ParseCommandLine (unsigned int args_count, const char** args, const char** env)
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
          else if (!xtl::xstrcmp (argument, KEY_SEARCH_PATH))
          {
            search_paths.Add (argument);
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

          //сохранение переменных среды при запуске
        
        common::FileSystem::Rename (environment_variables_file.Path (), STARTUP_ENV_VARS_FILE_NAME);

        common::XmlWriter writer (STARTUP_ENV_VARS_FILE_NAME);

        common::XmlWriter::Scope xml_root (writer, "Properties");
        
        if (env)
        {
          for (;*env; ++env)
          {
            const char* var   = *env;            
            const char* value = strchr (var, '=');
            
            if (value) value++;
            else       continue;
                        
            common::XmlWriter::Scope xml_property (writer, "Property");
            
            writer.WriteAttribute ("Name", stl::string (var, value-1).c_str ());
            writer.WriteAttribute ("Value", value);
          }
        }        

        return true;
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::ParseCommandLineEntry\n");
        fflush (stdout);        
      }      
      
      return false;
    }
    
///—оздание окна
    IWindow* CreateWindow (const char* name)
    {
      try
      {
        return create_window (name);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::Application::CreateWindow");
        throw;
      }
    }

///¬ыполнение приложени€
    void Run ()
    {
      try
      {
        if (!need_print_help && !need_print_version)
        {
            //загрузка лицензии

          common::Parser p (configuration_name.c_str ());

          if (p.Root ().First ("Configuration.LicenseFile"))
          {
            common::LicenseManager::Load (common::get<const char*> (p.Root ().First ("Configuration"), "LicenseFile"));
          }
          else
          {
            printf ("There is no license information in configuration\n");
            fflush (stdout);
          }
            
            //регистраци€ обработчика старта приложени€

          syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialized, xtl::bind (&Application::StartupHandler, this, p.Root ().First ("Configuration")));            
          
            //запуск основного цикла

          syslib::Application::Run ();          
        }
        else if (need_print_version)
        {
          common::Console::Printf ("Application version: %s\n", VERSION);
        }
        else if (need_print_help)
        {
          for (size_t i = 0, help_strings = sizeof (HELP) / sizeof (HELP[0]); i < help_strings; i++)
            common::Console::Print (HELP [i]);          
        }        
      }
      catch (std::exception& exception)
      {
        printf ("exception: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::RunEntry\n");
        fflush (stdout);
      }      
    }

    ///«апуск главного цикла
    void Execute (const char* command)
    {
      try
      {
        manager.Execute (command);
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::Execute: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::Execute\n");
        fflush (stdout);
      }
    }

  private:
///ќбработчик старта приложени€
    void StartupHandler (common::ParseNode config_node)
    {
      try
      {      
          //если основного цикла нет - выход из приложени€

        if (!has_main_loop)
          syslib::Application::Exit (0);            

        if (!need_print_help && !need_print_version)
        {
          try
          {
            common::Log log ("launcher");

            common::FileSystem::LogHandler log_handler (xtl::bind (&common::Log::Print, &log, _1));

            for (size_t i=0; i<search_paths.Size (); i++)
            {
              const char* path = search_paths [i];

              common::FileSystem::AddSearchPath (path, log_handler);
            }            

            manager.Start (config_node);            

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
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at engine::Application::StartupTimerHandler\n");
        fflush (stdout);        
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
    common::StringArray search_paths;               //пути поиска
    common::MemFile     environment_variables_file; //файл с переменными среды, указанными при запуске
};

}

/*
    ќбЄртки над обращени€ми к приложению
*/

extern "C" int MAKE_TARGET_LINK_INCLUDES_COMMA; 

static int touch [] = {MAKE_TARGET_LINK_INCLUDES_COMMA};

FUNNER_C_API IEngine* FunnerInit ()
{
  try
  {    
    return new Application;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
    fflush (stdout);
  }
  catch (...)
  {
    printf ("unknown exception at FunnerInit\n");
    fflush (stdout);
  }

  return 0;
}

/*
    Ёмул€ци€ запуска main функции
*/

FUNNER_C_API int FunnerMain (int argc, const char** argv, const char** env)
{
  IEngine* funner = FunnerInit ();

  if (!funner)
  {
    printf ("Funner startup failed!");
    return 1;
  }
  
  if (!funner->ParseCommandLine (argc, argv, env))
  {
    return 1;
  }

  funner->Run ();

  delete funner;
  
  return 0;
}
