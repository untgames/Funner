#ifndef CLIENT_CLIENT_SHARED_HEADER
#define CLIENT_CLIENT_SHARED_HEADER

#include <stl/algorithm>
#include <stl/auto_ptr.h>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/set>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>
#include <xtl/trackable.h>

#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <input/translation_map.h>

#include <client/engine.h>

namespace client
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера запуска
///////////////////////////////////////////////////////////////////////////////////////////////////
class StartupManagerImpl
{
  public:
    typedef client::StartupManager::StartupHandler StartupHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StartupManagerImpl ();
    ~StartupManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterStartupHandler       (const char* node_name, const StartupHandler& startup_handler, size_t order);
    void UnregisterStartupHandler     (const char* node_name);
    void UnregisterAllStartupHandlers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запуск обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Startup (Engine& engine, size_t low_level, size_t high_level, IEngineStartupParams* engine_startup_params);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

/*
   Синглтон менеджера запуска
*/

typedef common::Singleton<StartupManagerImpl> StartupManagerSingleton;

}

#endif
