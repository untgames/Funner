#include "shared.h"

using namespace engine;

namespace
{

/*
    Константы
*/

const char* LOG_PREFIX               = "engine";               //имя потока протоколирования
const char* REGISTRY_COMPONENTS_MASK = "engine.subsystems.*";  //маска имени автоматически загружаемых компонентов

}

/*
    Описание реализации менеджера запуска подсистем
*/

struct StartupManagerImpl::Impl
{
  public:
///Регистрация обработчика запуска подсистемы
    void RegisterStartupHandler (const char* configuration_node_name, const StartupHandler& startup_handler)
    {
      static const char* METHOD_NAME = "client::StartupManager::RegisterStartupHandler";

      if (!configuration_node_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "configuration_node_name");

      StartupHandlers::iterator iter = startup_handlers.find (configuration_node_name);

      if (iter != startup_handlers.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "configuration_node_name", configuration_node_name, "Startup handler has already registered");

      startup_handlers.insert_pair (configuration_node_name, StartupHandlerEntryPtr (new StartupHandlerEntry (configuration_node_name, startup_handler), false));
    }

///Удаление обработчика запуска подсистемы
    void UnregisterStartupHandler (const char* configuration_node_name)
    {
      if (!configuration_node_name)
        return;

      startup_handlers.erase (configuration_node_name);
    }

///Удаление всех обработчиков запуска подсистем
    void UnregisterAllStartupHandlers ()
    {
      startup_handlers.clear ();
    }

///Запуск подсистем
    void Start (common::ParseNode& node, const char* wc_mask, SubsystemManager& manager)
    {
      static common::ComponentLoader loader (REGISTRY_COMPONENTS_MASK);

      common::Log log (common::format ("%s.%s", LOG_PREFIX, manager.Name ()).c_str ());

      log.Printf ("Start subsystems...");

      for (common::ParseNode iter=node.First (); iter; iter=iter.Next ())
      {
        const char* node_name = iter.Name ();

        if (!common::wcmatch (node_name, wc_mask))
          continue;

          //поиск обработчика запуска

        StartupHandlers::iterator startup_iter = startup_handlers.find (node_name);

        if (startup_iter == startup_handlers.end ())
        {
          log.Printf ("Unknown configuration node '%s'", node_name);
          continue;
        }

          //старт подсистем

        StartupHandlerEntry& entry = *startup_iter->second;

        try
        {
          entry.handler (iter, manager);
        }
        catch (std::exception& exception)
        {
          log.Printf ("Exception %s(%u): node '%s': %s\n", iter.Source (), iter.LineNumber (), node_name, exception.what ());
        }
        catch (...)
        {
          log.Printf ("Exception %s(%u): node '%s': unknown exception\n", iter.Source (), iter.LineNumber (), node_name);
        }
      }

      log.Printf ("Subsystems successfully started");
    }

  private:
    struct StartupHandlerEntry: public xtl::reference_counter
    {
      stl::string    node_name;
      StartupHandler handler;

      StartupHandlerEntry (const char* in_node_name, const StartupHandler& in_handler)
        : node_name (in_node_name), handler (in_handler)
        {}
    };

    typedef xtl::intrusive_ptr<StartupHandlerEntry>                           StartupHandlerEntryPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, StartupHandlerEntryPtr> StartupHandlers;

  private:
    StartupHandlers startup_handlers;
};

/*
    Конструктор / деструктор
*/

StartupManagerImpl::StartupManagerImpl ()
  : impl (new Impl)
  {}

StartupManagerImpl::~StartupManagerImpl ()
{
}

/*
    Добавление / удаление обработчиков
*/

void StartupManagerImpl::RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler)
{
  impl->RegisterStartupHandler (node_name, startup_handler);
}

void StartupManagerImpl::UnregisterStartupHandler (const char* node_name)
{
  impl->UnregisterStartupHandler (node_name);
}

void StartupManagerImpl::UnregisterAllStartupHandlers ()
{
  impl->UnregisterAllStartupHandlers ();
}

/*
    Запуск обработчиков
*/

void StartupManagerImpl::Start (common::ParseNode& node, const char* wc_mask, SubsystemManager& manager)
{
  impl->Start (node, wc_mask, manager);
}

/*
    Обёртки над вызовами методов StartupManagerImpl
*/

void StartupManager::RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler)
{
  StartupManagerSingleton::Instance ()->RegisterStartupHandler (node_name, startup_handler);
}

void StartupManager::UnregisterStartupHandler (const char* node_name)
{
  StartupManagerSingleton::Instance ()->UnregisterStartupHandler (node_name);
}

void StartupManager::UnregisterAllStartupHandlers ()
{
  StartupManagerSingleton::Instance ()->UnregisterAllStartupHandlers ();
}
