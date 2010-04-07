#include <xtl/shared_ptr.h>

#include <script/bind.h>
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

/*
    Подсистема скриптового интерпретатора
*/

class ShellSubsystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    ShellSubsystem (common::ParseNode& node, SubsystemManager& manager)
      : environment (new Environment)
    {
        //чтение конфигурации

      const char *interpreter = get<const char*> (node, "Interpreter"),
                 *libraries   = get<const char*> (node, "Libraries", ""),
                 *sources     = get<const char*> (node, "Sources", ""),
                 *command     = get<const char*> (node, "Execute", "");

      StringArray lib_list = split (libraries),
                  src_list = split (sources);

        //загрузка библиотек

      for (size_t i=0; i<lib_list.Size (); i++)
        environment->BindLibraries (lib_list [i]);

        //регистрация менеджера подсистем

      InvokerRegistry& engine_lib = environment->Library ("Engine");

      engine_lib.Register ("get_SubsystemManager", make_const (xtl::ref (manager)));

        //создание интерпретатора

      Shell (interpreter, environment).Swap (shell);

        //загрузка и исполнение исходных файлов

      for (size_t i=0; i<src_list.Size (); i++)
        shell.ExecuteFileList (src_list [i]);

        //исполнение команды

      if (*command)
        shell.Execute (command);
    }
    
///Выполнение команды
    void Execute (const char* command)
    {
      if (!wcimatch (command, "*.lua") && !wcimatch (command, "*.luac"))
        return;                
        
      try
      {
        shell.ExecuteFile (command);
      }
      catch (xtl::exception& e)
      { 
        e.touch ("engine::ShellSubsystem::Execute");
        throw;
      }
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    typedef Shell::EnvironmentPtr EnvironmentPtr;

  private:
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
        xtl::com_ptr<ShellSubsystem> subsystem (new ShellSubsystem (node, manager), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
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
