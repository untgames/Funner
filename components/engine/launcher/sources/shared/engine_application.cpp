#include "shared.h"

using namespace engine;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

#ifndef LAUNCHER_VERSION
  #define LAUNCHER_VERSION "unknown version"
#endif

/*
    Константы
*/

const char* VERSION                         = LAUNCHER_VERSION;   //строка версии
const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";       //имя конфигурационного файла по умолчанию
const bool  DEFAULT_HAS_MAIN_LOOP           = true;               //наличие главного цикла приложения по умолчанию
const char* STARTUP_ENV_VARS_FILE_NAME      = "/anonymous/system/startup_environment_variables.xml"; //имя файла с переменными среды при запуске

const char* KEY_CONFIGURATION = "--config=";      //имя ключа конфигурационного файла
const char* KEY_SEARCH_PATH   = "--search-path="; //путь к каталогу поиска файлов
const char* KEY_NO_MAIN_LOOP  = "--no-main-loop"; //отключение главного цикла приложения
const char* KEY_MAIN_LOOP     = "--main-loop";    //включение главного цикла приложения
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
    Приложение
*/

class Application: public IEngine
{
  public:
///Конструктор
    Application ()
      : environment_variables_file ((size_t)0, common::FileMode_ReadWrite | common::FileMode_Create)
      , notification_connection (syslib::Application::RegisterNotificationHandler ("*", xtl::bind (&Application::OnNotification, this, _1)))
      , main_thread_id (syslib::Thread::GetCurrentThreadId ())
    {
      has_main_loop      = DEFAULT_HAS_MAIN_LOOP;
      configuration_name = DEFAULT_CONFIGURATION_FILE_NAME;
      need_print_version = false;
      need_print_help    = false;
    }
    
    ///Установка базовой директории
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
    
    ///Получение базовой директории
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
        
///Разбор параметров командой строки
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
    
///Создание окна
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

///Выполнение приложения
    void Run ()
    {
      try
      {
        if (syslib::Thread::GetCurrentThreadId () != main_thread_id)
          throw xtl::format_operation_exception ("", "Run must be called only from the thread in which object was created");

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
            
            //регистрация обработчика старта приложения

          syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, xtl::bind (&Application::StartupHandler, this, p.Root ().First ("Configuration")));            
          
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
        printf ("exception at Application::RunEntry: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::RunEntry\n");
        fflush (stdout);
      }      
    }

    ///Выполнение внешних комманд
    void Execute (const char* command)
    {
      if (syslib::Thread::GetCurrentThreadId () == main_thread_id)
        ExecuteImpl (command);
      else
        common::ActionQueue::PushAction (xtl::bind (&Application::ExecuteOnMainThread, this, stl::string (command)), common::ActionThread_Main);
    }

    ///Посылка оповещения
    void PostNotification (const char* notification)
    {
      try
      {
        if (!notification)
          throw xtl::make_null_argument_exception ("", "notification");

        syslib::Application::PostNotification (notification);
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::PostNotification: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::PostNotification\n");
        fflush (stdout);
      }
    }

    ///Добавление слушателя сообщений
    void AttachNotificationListener (const char* notification_wildcard, INotificationListener* listener)
    {
      try
      {
        if (!notification_wildcard)
          throw xtl::make_null_argument_exception ("", "notification_wildcard");

        if (!listener)
          throw xtl::make_null_argument_exception ("", "listener");

        notification_listeners.push_back (NotificationListenerDescPtr (new NotificationListenerDesc (notification_wildcard, listener), false));
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::AttachNotificationListener: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::AttachNotificationListener\n");
        fflush (stdout);
      }
    }

    ///Удаление слушателя сообщений
    void DetachNotificationListener (const char* notification_wildcard, INotificationListener* listener)
    {
      try
      {
        if (!notification_wildcard)
          return;

        if (!listener)
          return;

        for (NotificationListeners::iterator iter = notification_listeners.begin (), end = notification_listeners.end (); iter != end;)
        {
          if ((*iter)->notification_wildcard == notification_wildcard && (*iter)->listener == listener)
          {
            NotificationListeners::iterator next = iter;

            ++next;

            notification_listeners.erase (iter);

            iter = next;
          }
          else
            ++iter;
        }
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::DetachNotificationListener: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::DetachNotificationListener\n");
        fflush (stdout);
      }
    }

    void DetachNotificationListener (INotificationListener* listener)
    {
      try
      {
        if (!listener)
          return;

        for (NotificationListeners::iterator iter = notification_listeners.begin (), end = notification_listeners.end (); iter != end;)
        {
          if ((*iter)->listener == listener)
          {
            NotificationListeners::iterator next = iter;

            ++next;

            notification_listeners.erase (iter);

            iter = next;
          }
          else
            ++iter;
        }
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::DetachNotificationListener: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::DetachNotificationListener\n");
        fflush (stdout);
      }
    }

    ///Удаление всех слушателей сообщений
    void DetachAllNotificationListeners ()
    {
      try
      {
        notification_listeners.clear ();
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::DetachAllNotificationListeners: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::DetachAllNotificationListeners\n");
        fflush (stdout);
      }
    }

  private:
///Выполнение внешних комманд
    void ExecuteImpl (const char* command)
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

    void ExecuteOnMainThread (const stl::string& command)
    {
      ExecuteImpl (command.c_str ());
    }

///Обработчик оповещений приложения
    void OnNotification (const char* notification)
    {
      try
      {
        for (NotificationListeners::iterator iter = notification_listeners.begin (), end = notification_listeners.end (); iter != end; ++iter)
        {
          if (common::wcmatch (notification, (*iter)->notification_wildcard.c_str ()))
            (*iter)->listener->OnNotification (notification);
        }
      }
      catch (std::exception& exception)
      {
        printf ("exception at Application::OnNotification: %s\n", exception.what ());
        fflush (stdout);
      }
      catch (...)
      {
        printf ("unknown exception at Application::OnNotification\n");
        fflush (stdout);
      }
    }

///Обработчик старта приложения
    void StartupHandler (common::ParseNode config_node)
    {
      try
      {      
          //если основного цикла нет - выход из приложения

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
    struct NotificationListenerDesc : public xtl::reference_counter
    {
      stl::string            notification_wildcard;
      INotificationListener* listener;

      NotificationListenerDesc (const char *in_notification_wildcard, INotificationListener *in_listener)
        : notification_wildcard (in_notification_wildcard)
        , listener (in_listener)
        {}
    };

    typedef xtl::intrusive_ptr<NotificationListenerDesc> NotificationListenerDescPtr;
    typedef stl::list<NotificationListenerDescPtr>       NotificationListeners;

  private:
    SubsystemManager           manager;                    //менеджер подсистем
    bool                       has_main_loop;              //есть ли главный цикл приложения
    stl::string                configuration_name;         //имя конфигурации
    bool                       need_print_version;         //нужно распечатать строку версии
    bool                       need_print_help;            //нужно распечатать помощь по запуску приложения
    common::StringArray        commands;                   //команды на выполнение подсистемами
    common::StringArray        search_paths;               //пути поиска
    common::MemFile            environment_variables_file; //файл с переменными среды, указанными при запуске
    NotificationListeners      notification_listeners;     //слушатели оповещений приложения
    xtl::auto_connection       notification_connection;    //соедениние оповещений приложения
    syslib::Thread::threadid_t main_thread_id;             //идентификатор главной нити
};

}

/*
    Обёртки над обращениями к приложению
*/

//extern "C" int MAKE_TARGET_LINK_INCLUDES_COMMA; 

//static int touch [] = {MAKE_TARGET_LINK_INCLUDES_COMMA};

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
    Эмуляция запуска main функции
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
