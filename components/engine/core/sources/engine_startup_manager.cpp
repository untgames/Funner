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
///Конструктор
    Impl ()
    {
    }
  
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
    void Start (const common::ParseNode& node, const char* subsystems_name_mask, SubsystemManager& manager)
    {
      static common::ComponentLoader loader (REGISTRY_COMPONENTS_MASK);
      
      common::Log log (common::format ("%s.%s", LOG_PREFIX, manager.Name ()).c_str ());      

      log.Printf ("Start subsystems...");      
      
      common::PropertyMap properties;

      StartCore (node, subsystems_name_mask, manager, properties);

      log.Printf ("Subsystems successfully started");
    }

  private:
///Запуск подсистем
    void StartCore (const common::ParseNode& node, const char* subsystems_name_mask, SubsystemManager& manager, common::PropertyMap& properties)
    {      
      common::Log log (common::format ("%s.%s", LOG_PREFIX, manager.Name ()).c_str ());      
      
      common::StringArray masks = common::split (subsystems_name_mask);
      
      for (common::ParseNode iter=node.First (); iter; iter=iter.Next ())
      {
        const char* node_name = iter.Name ();
        
        if (!strcmp (node_name, "Include"))
        {
          try
          {
            IncludeSubsystems (iter, subsystems_name_mask, manager, properties);
          }
          catch (std::exception& exception)
          {
            log.Printf ("Exception %s(%u): node '%s': %s", iter.Source (), iter.LineNumber (), node_name, exception.what ());
          }
          catch (...)
          {
            log.Printf ("Exception %s(%u): node '%s': unknown exception", iter.Source (), iter.LineNumber (), node_name);
          }

          continue;
        }
        else if (!strcmp (node_name, "Properties"))
        {                    
          ReadProperties (iter, properties, log);
          
          continue;
        }

        bool match = false;
        
        for (size_t i=0; i<masks.Size (); i++)          
          if (common::wcmatch (node_name, masks [i]))
          {
            match = true;
            break;
          }
          
        if (!match)
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
          common::ParseNode parse_node = resolve_references (iter, properties);

          entry.handler (parse_node, manager);
        }
        catch (std::exception& exception)
        {
          log.Printf ("Exception %s(%u): node '%s': %s", iter.Source (), iter.LineNumber (), node_name, exception.what ());
        }
        catch (...)
        {
          log.Printf ("Exception %s(%u): node '%s': unknown exception", iter.Source (), iter.LineNumber (), node_name);
        }
      }
    }  
  
///Подключение подсистем, описанных в другом файле
    void IncludeSubsystems (common::ParseNode& node, const char* subsystems_name_mask, SubsystemManager& manager, common::PropertyMap& properties)
    {
      try
      {
        const char* file_name             = common::get<const char*> (node, "Source");        
        bool        ignore_unavailability = common::get<int> (node, "IgnoreUnavailability", 0) == 1;
        
        if (!common::FileSystem::IsFileExist (file_name) && ignore_unavailability)
          return;

        common::Parser parser (file_name);
        common::Log log (LOG_PREFIX);

        parser.Log ().Print (xtl::bind (&common::Log::Print, &log, _1));

        common::ParseNode configuration_node = parser.Root ().First ("Configuration");

        StartCore (configuration_node, subsystems_name_mask, manager, properties);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::StartupManagerImpl::IncludeHandler");
        throw;
      }
    }
    
///Чтение свойств
    void ReadProperties (common::ParseNode& node, common::PropertyMap& properties, common::Log& log)
    {
      for (common::Parser::Iterator iter=node.First ("Property"); iter; ++iter)
      {
        try
        {
          const char* name  = common::get<const char*> (*iter, "Name");
          const char* value = common::get<const char*> (*iter, "Value");
          
          properties.SetProperty (name, value);
        }
        catch (std::exception& exception)
        {
          log.Printf ("Exception %s(%u): %s", iter->Source (), iter->LineNumber (), exception.what ());
        }
        catch (...)
        {
          log.Printf ("Exception %s(%u): unknown exception", iter->Source (), iter->LineNumber ());
        }
      }
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

void StartupManagerImpl::Start (const common::ParseNode& node, const char* subsystems_name_mask, SubsystemManager& manager)
{
  impl->Start (node, subsystems_name_mask, manager);
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
