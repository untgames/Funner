#include <xtl/shared_ptr.h>

#include <script/environment.h>
#include <script/shell.h>

#include "shared.h"

using namespace engine;
using namespace script;
using namespace common;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "Shell";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.Shell"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;            //имя потока протоколирования

/*
    Подсистема скриптового интерпретатора
*/

struct ShellLogHandler
{
  ShellLogHandler (Log& in_log) : log (in_log), was_error (false) {}

  void operator () (const char* message)
  {
    was_error = true;

    log.Print (message);
  }

  Log& log;
  bool was_error;
};

class ShellSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    ShellSubsystem (common::ParseNode& node)
      : log (COMPONENT_NAME),
        environment (new Environment)
    {
      static const char* METHOD_NAME = "engine::subsystems::ShellSubsystem::ShellSubsystem";
        
        //чтение конфигурации
      
      const char *interpreter = get<const char*> (node, "Interpreter"),
                 *libraries   = get<const char*> (node, "Libraries", ""),
                 *sources     = get<const char*> (node, "Sources"),
                 *command     = get<const char*> (node, "Execute", ""),
                 *log_name    = get<const char*> (node, "Log", "");

      StringArray lib_list = split (libraries),
                  src_list = split (sources);

        //установка потока протоколирования

      if (*log_name)
        Log (log_name).Swap (log);
        
        //загрузка библиотек
        
      for (size_t i=0; i<lib_list.Size (); i++)
        environment->BindLibraries (lib_list [i]);
        
        //создание интерпретатора
        
      Shell (interpreter, environment).Swap (shell);

        //загрузка и исполнение исходных файлов

      ShellLogHandler shell_log_handler (log);

      Shell::LogFunction log_handler (xtl::ref (shell_log_handler));

      for (size_t i=0; i<src_list.Size (); i++)
      {
        shell.ExecuteFileList (src_list [i], log_handler);

        if (shell_log_handler.was_error)
          throw xtl::format_operation_exception (METHOD_NAME, "Lua exception while executing file list '%s'", src_list[i]);
      }  

        //исполнение команды
        
      if (*command)
        shell.Execute (command, log_handler);

      if (shell_log_handler.was_error)
        throw xtl::format_operation_exception (METHOD_NAME, "Lua exception while executing command '%s'", command);
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
    
  private:
    typedef Shell::EnvironmentPtr EnvironmentPtr;

  private:
    Log            log;         //протокол подсистемы
    EnvironmentPtr environment; //скриптовое окружение
    Shell          shell;       //скриптовый интерпретатор
};

/*
    Компонент регистрации
*/

class Component
{
  public:
    Component () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ShellSubsystem> subsystem (new ShellSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::ShellComponent::StartupHandler");
        throw;
      }      
    }
};

extern "C"
{

ComponentRegistrator<Component> ShellSubsystem (COMPONENT_NAME);

}

}
