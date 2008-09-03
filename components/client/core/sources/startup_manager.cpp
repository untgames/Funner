#include "shared.h"

using namespace client;

namespace
{

const char* LOG_NAME = "client.StartupManager";

}

struct StartupManagerImpl::Impl
{
  public:
    Impl () : need_sort (false), log (LOG_NAME) {}

    void RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler, int order)
    {
      static const char* METHOD_NAME = "client::StartupManager::RegisterStartupHandler";

      if (!node_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "node_name");

      StartupHandlers::iterator iter = FindStartupHandler (node_name);

      if (iter != startup_handlers.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't register startup handler with node name '%s', already registered", node_name);

      startup_handlers.push_back (StartupHandlerEntryPtr (new StartupHandlerEntry (node_name, startup_handler, order)));

      need_sort = true;
    }
    
    void UnregisterStartupHandler (const char* node_name)
    {
      if (!node_name)
        throw xtl::make_null_argument_exception ("client::StartupManager::UnregisterStartupHandler", "node_name");

      StartupHandlers::iterator iter = FindStartupHandler (node_name);

      if (iter != startup_handlers.end ())
        startup_handlers.erase (iter);
    }

    void UnregisterAllStartupHandlers ()
    {
      startup_handlers.clear ();
    }

    void Startup (Engine& engine, IEngineStartupParams* engine_startup_params)
    {
      if (need_sort)
      {
        stl::sort (startup_handlers.begin (), startup_handlers.end (), xtl::bind (&Impl::StartupHandlerEntryLessPredicate, this, _1, _2));
        need_sort = false;
      }

      stl::hash_set<stl::hash_key<const char*>> registry_branches;

      common::VarRegistry configuration_registry (engine.ConfigurationBranch ());

      for (common::VarRegistry::Iterator configuration_branch_iter = configuration_registry.CreateIterator (); configuration_branch_iter; ++configuration_branch_iter)
        registry_branches.insert (configuration_branch_iter->Name ());

      for (StartupHandlers::iterator iter = startup_handlers.begin (), end = startup_handlers.end (); iter != end; ++iter)
      {
        if (!registry_branches.count ((*iter)->node_name.c_str ()))
        {
          log.Printf ("Startup handler with node name '%s' was not called, no such branch in registry '%s'", (*iter)->node_name.c_str (), engine.ConfigurationBranch ());
          continue;
        }

        if (*engine.ConfigurationBranch ())
          configuration_registry.Open (common::format ("%s.%s", engine.ConfigurationBranch (), (*iter)->node_name.c_str ()).c_str ());
        else
          configuration_registry.Open ((*iter)->node_name.c_str ());

        (*iter)->handler (configuration_registry, engine_startup_params, engine);
      }
    }

  private:
    struct StartupHandlerEntry
    {
      stl::string    node_name;
      StartupHandler handler;
      int            order;

      StartupHandlerEntry (const char* in_node_name, const StartupHandler& in_handler, int in_order)
        : node_name (in_node_name), handler (in_handler), order (in_order)
        {}
    };

    typedef xtl::shared_ptr<StartupHandlerEntry> StartupHandlerEntryPtr;
    typedef stl::vector<StartupHandlerEntryPtr>  StartupHandlers;

  private:
    bool StartupHandlerEntryLessPredicate (const StartupHandlerEntryPtr& left, const StartupHandlerEntryPtr& right)
    {
      return left->order < right->order;
    }

    StartupHandlers::iterator FindStartupHandler (const char* node_name)
    {
      for (StartupHandlers::iterator iter = startup_handlers.begin (), end = startup_handlers.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->node_name.c_str (), node_name))
          return iter;

      return startup_handlers.end ();
    }

  private:
    StartupHandlers startup_handlers;
    bool            need_sort;
    common::Log     log;
};

/*
   Реализация менеджера запуска
*/

/*
   Конструктор/деструктор
*/

StartupManagerImpl::StartupManagerImpl ()
  : impl (new Impl)
  {}

StartupManagerImpl::~StartupManagerImpl ()
{
}

/*
   Добавление/удаление обработчиков
*/

void StartupManagerImpl::RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler, int order)
{
  impl->RegisterStartupHandler (node_name, startup_handler, order);
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

void StartupManagerImpl::Startup (Engine& engine, IEngineStartupParams* engine_startup_params)
{
  impl->Startup (engine, engine_startup_params);
}

/*
   Менеджер запуска
*/

/*
   Добавление/удаление обработчиков
*/

void StartupManager::RegisterStartupHandler (const char* node_name, const StartupHandler& startup_handler, int order)
{
  StartupManagerSingleton::Instance ().RegisterStartupHandler (node_name, startup_handler, order);
}

void StartupManager::UnregisterStartupHandler (const char* node_name)
{
  StartupManagerSingleton::Instance ().UnregisterStartupHandler (node_name);
}

void StartupManager::UnregisterAllStartupHandlers ()
{
  StartupManagerSingleton::Instance ().UnregisterAllStartupHandlers ();
}
